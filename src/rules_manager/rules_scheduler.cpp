/*
 * AquaBox
 * Copyright (C) 2019-2022  Eduard Sargsyan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "rules_scheduler.h"
#include "rules_map.h"
#include <common/log.h>
#include <common/singleton.h>
#include <micro_clients/devices_manager.h>
#include <micro_clients/events.h>
#include <types/rule.h>
#include <types/types.h>
#include <types/update_events.h>

#define MAGIC_TEMPORARY_PROPERTY "__remain__"

using namespace aquabox;

RuleScheduler::RuleScheduler() {}

void RuleScheduler::recover()
{
    LOG_INFO << "RuleSchedulering devices states";
    Singleton<RulesMap>::i()->for_each([this](std::string id, std::shared_ptr<Rule> rule) { add(*rule); });
    LOG_INFO << "Starting Rules service";
    start();
}

void RuleScheduler::add(const Rule& rule)
{
    if (!rule.enabled()) {
        return;
    }
    for (auto& vc : rule.actions()) {
        std::pair<std::string, ValueChange> data({rule.id(), vc});
        int id = Scheduler<std::pair<std::string, ValueChange>>::add(vc.cron, data, rule.lastRun());
        if (id < 0) {
            LOG_ERROR << "Can't add action for rule " << rule.id();
        }
        else {
            LOG_DEBUG << "Added action of rule " << rule.id() << " as task " << id;
        }
    }

    if (rule.isTemporary()) {
        ValueChange remainValue;
        remainValue.property = MAGIC_TEMPORARY_PROPERTY;
        remainValue.value = "next";
        std::pair<std::string, ValueChange> data({rule.id(), remainValue});
        Scheduler<std::pair<std::string, ValueChange>>::add("* * * * * *", data, 0);
    }
}

void RuleScheduler::begin()
{
    m_changes.clear();
}

void RuleScheduler::finish()
{
    for (auto& change : m_changes) {
        if (!Singleton<IDevicesManager>::i()->rulesEnabled(change.first)) {
            LOG_INFO << "Rules disabled on device " << change.first;
        }
        if (!Singleton<IDevicesManager>::i()->set(change.first, change.second.property, change.second.value)) {
            LOG_ERROR << "Can't apply change on " << change.first;
        }
    }

    for (auto r : m_rulesToDelete) {
        Singleton<RulesMap>::i()->removeRule(r);
    }
}

bool RuleScheduler::ready(const int& id, const std::pair<std::string, ValueChange>& data)
{
    auto r = Singleton<RulesMap>::i()->get(data.first, nullptr);

    if (r == nullptr) {
        LOG_DEBUG << "Action " << id << " is ready. "
                  << "Rule \"" << data.first << "\" for property \"" << data.second.property << "\" but is missing";
        return false;
    }

    LOG_DEBUG << "Action " << id << " is ready. "
              << "Rule \"" << data.first << "\" for property \"" << data.second.property << "\", should set is to " << data.second.value.dump();

    if (r->isTemporary() && data.second.property == MAGIC_TEMPORARY_PROPERTY) {
        Singleton<IEvents>::i()->notify(std::make_shared<TemporaryRuleRemainEvent>(*r));
    }
    else if (r->enabled()) {
        r->setLastRun(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
        Singleton<RulesMap>::i()->saveRule(data.first);

        m_changes[r->device()] = data.second;
    }

    if (r->expired()) {
        LOG_VERBOSE << "Rule " << data.first << " has expired. Scheduling for delete";
        m_rulesToDelete.insert(data.first);
        return false;
    }

    return true;
}

void RuleScheduler::remove(const Rule& rule)
{
    std::string ruleId = rule.id();
    LOG_DEBUG << "Removing actions for rule " << ruleId;
    int count = 0;
    removeIf([ruleId, &count](const int& id, std::pair<std::string, ValueChange>& vc) { ++count; return vc.first == ruleId; });
    LOG_DEBUG << "Removed " << count << " actions from for rule " << ruleId;
}

void RuleScheduler::updateDeviceRules(const std::string& deviceId)
{
    time_t rawtime;

    time (&rawtime);
    struct tm* timeinfo = localtime (&rawtime);
    timeinfo->tm_hour = 0;
    timeinfo->tm_min = 0;
    timeinfo->tm_sec = 0;
    rawtime = mktime (timeinfo);

    beginTransaction();

    bool reAdd = Singleton<IDevicesManager>::i()->rulesEnabled(deviceId);

    Singleton<RulesMap>::i()->for_each([this, deviceId, rawtime, reAdd] (const std::string id, std::shared_ptr<Rule>& rule) {
        if (rule->device() == deviceId && rule->enabled()) {
            remove(*rule);
            if (reAdd) {
                rule->setLastRun(rawtime);
                add(*rule);
            }
        }
    });

    endTransaction();
}
