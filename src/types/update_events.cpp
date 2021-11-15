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

#include "update_events.h"
#include <types/types.h>
#include <types/rule.h>
#include <chrono>
#include <common/enumerations.h>

using namespace aquabox;


RuleEnableChangedEvent::RuleEnableChangedEvent(std::shared_ptr<Rule> rule)
{
    setSender(rule->id(), "rule");
    setData({
        { "sender_full", rule->name() },
        { "device", rule->device() },
    });

    setPropertiesUpdate({ { "enabled", rule->enabled() } });
}

RuleCreatedEvent::RuleCreatedEvent(const Rule& rule)
{
    setSender(rule.id(), "rule");
    setData({ { "sender_full", rule.name() },
        { "device", rule.device() },
        { "action", "created" } });

    nlohmann::json json = rule;
    setPropertiesUpdate({ { "*", json } });
}

RuleUpdatedEvent::RuleUpdatedEvent(const Rule& rule,
    const nlohmann::json& before)
{
    setSender(rule.id(), "rule");
    setData({ { "sender_full", rule.name() },
        { "device", rule.device() },
        { "action", "updated" } });

    nlohmann::json json = rule;

    nlohmann::json res;
    for (auto i : json.items()) {
        auto p = before.find(i.key());
        if (p != before.end()) {
            if (p.value() == i.value()) {
                continue;
            }
        }
        res[i.key()] = i.value();
    }

    setPropertiesUpdate(res);
}

TemporaryRuleRemainEvent::TemporaryRuleRemainEvent(const Rule& rule)
{
    setSender(rule.id(), "rule");
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto deleteAt = rule.createdAt() + rule.deleteAfter();
    auto remain = (now < deleteAt) ? (deleteAt - now) : 0;
    setData({ { "remain", remain } });
}

RuleDeletedEvent::RuleDeletedEvent(const Rule& rule)
{
    setSender(rule.id(), "rule");
    setData({ { "sender_full", rule.name() },
        { "device", rule.device() },
        { "action", "deleted" } });
}

DeviceRulesChangedEvent::DeviceRulesChangedEvent(const DeviceRef& device)
{
    setSender(device.id(), "device");
    setData({
        { "sender_full", device.name() },
    });
/*
    std::vector<nlohmann::json> rules;
    for (auto r : device->rules()) {
        nlohmann::json ruleJson = *r;
        rules.push_back(ruleJson);
    }

    setPropertiesUpdate({ { "rules", rules } });*/
}
