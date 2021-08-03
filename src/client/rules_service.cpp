/*
 * AquaBox
 * Copyright (C) 2019  Eduard Sargsyan
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

#include "rules_service.h"
#include "../types/rule.h"
#include "../types/device.h"
#include "devices_map.h"
#include "aquabox.h"
#include "events/update_events.h"
#include <log.h>

using namespace aquabox;

RulesService::RulesService()
{}

void RulesService::recover()
{
    LOG_INFO << "Recovering devices states";
    Singleton<AquaBox>::i()->rules().for_each([this](std::string id, std::shared_ptr<Rule> rule) {
        add(*rule);
    });
    LOG_INFO << "Starting Rules service";
    start();
}

void RulesService::add(const Rule& rule)
{
    if (!rule.enabled()) {
        return;
    }
    for(auto& vc : rule.actions()) {
        std::pair<std::string, ValueChange> data({rule.id(), vc});
        int id = Scheduler<std::pair<std::string, ValueChange>>::add(vc.cron, data, rule.lastRun());
        if(id < 0) {
            LOG_ERROR << "Can't add action for rule " << rule.id();
        } else {
            LOG_DEBUG << "Added action of rule " << rule.id() << " as task " << id;
        }
    }

    if (rule.isTemporary()) {
        ValueChange remainValue;
        remainValue.property = "remain";
        std::pair<std::string, ValueChange> data({rule.id(), remainValue});
        Scheduler<std::pair<std::string, ValueChange>>::add("* * * * * *", data, rule.lastRun());
    }
}

void RulesService::begin()
{
    m_changes.clear();
}

void RulesService::finish()
{
    for (auto& change : m_changes)
    {
        auto dev = Singleton<DevicesMap>::i()->get(change.first, nullptr);
        LOG_INFO << "Applying rule change for device " << change.first << " set " << change.second.property << " to " << change.second.value.dump();
        if (dev != nullptr) {
            if (dev->value(change.second.property) != change.second.value) {
                dev->setValue(change.second.property, change.second.value);
                Singleton<AquaBox>::i()->postUpdate(std::make_shared<ValueChangedEvent>(dev, change.second.property, change.second.value, "rule"));
            }
        } else {
            LOG_ERROR << "Can't find device " << change.first;
        }
    }

    for (auto r : m_rulesToDelete)
    {
        auto rule = Singleton<AquaBox>::i()->rules().get(r, nullptr);
        if (rule != nullptr) {
            remove(*rule);
        }
        Singleton<AquaBox>::i()->removeRule(r);
    }
}

bool RulesService::ready(const int& id, std::pair<std::string, ValueChange>& data)
{
    LOG_DEBUG << "Action " << id << " is ready";
    auto r = Singleton<AquaBox>::i()->rules().get(data.first, nullptr);

    if (r == nullptr) {
        return false;
    }

    if (r->isTemporary() && data.second.property == "remain") {
        Singleton<AquaBox>::i()->postUpdate(std::make_shared<TemporaryRuleRemainEvent>(r));
    } else if (r->enabled()) {
        r->setLastRun(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
        Singleton<AquaBox>::i()->saveRule(data.first);

        m_changes[r->device()] = data.second;
    }

    if (r->expired()) {
        LOG_VERBOSE << "Rule " << data.first << " has expired. Scheduling for delete";
        m_rulesToDelete.insert(data.first);
        return false;
    }

    return true;
}

void RulesService::remove(const Rule& rule)
{
    std::string ruleId = rule.id();
    LOG_DEBUG << "Removing actions for rule " << ruleId;
    removeIf([ruleId](const int& id, std::pair<std::string, ValueChange>& vc) {
        return vc.first == ruleId;
    });
}
