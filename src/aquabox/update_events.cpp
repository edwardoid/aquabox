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
#include "relay.h"
#include "sensor.h"
#include "rule.h"
#include "enumerations.h"
#include "serial.h"

using namespace aquabox;

BoxEvent::BoxEvent()
{
    setSender(SERIAL_NUMBER, "aquabox");
}

OnOffChangedEvent::OnOffChangedEvent(std::shared_ptr<Relay> device, std::string reason)
{
    setSender(device->getId(), "device");
    setData({
        { "reason", reason },
        { "sender_full", device->getName().Value },
        { "action", Serializable<ActionType>::convert(device->turnedOn() ?  ActionType::TurnOff : ActionType::TurnOn) }
    });

    setPropertiesUpdate({
        { "isOn", device->turnedOn() }
    });
}

SensorChangedEvent::SensorChangedEvent(std::shared_ptr<Sensor> sensor, std::string reason)
{
    setSender(sensor->getId(), "device");
    setData({
        { "reason", reason },
        { "sender_full", sensor->getName().Value },
    });

    setPropertiesUpdate({
        { "value", sensor->value() }
    });
}

RuleTriggeredEvent::RuleTriggeredEvent(std::shared_ptr<Relay> device)
    : OnOffChangedEvent(device, "rule")
{
}

ManualStateChangedEvent::ManualStateChangedEvent(std::shared_ptr<Relay> device)
    : OnOffChangedEvent(device, "manual")
{
}

RuleEnableChangedEvent::RuleEnableChangedEvent(std::shared_ptr<Rule> rule)
{
    setSender(rule->getId(), "rule");
    setData({
        { "sender_full", rule->getName().Value },
        { "device", rule->getDevice().Value }
    });

    setPropertiesUpdate({
        { rule->getEnabled().name, rule->getEnabled().Value }
    });
}

RuleCreatedEvent::RuleCreatedEvent(std::shared_ptr<Rule> rule)
{
    setSender(rule->getId(), "rule");
    setData({
        { "sender_full", rule->getName().Value },
        { "device", rule->getDevice().Value },
        { "action", "created"}
    });

    nlohmann::json json;
    rule->json(json);
    setPropertiesUpdate({
        { "*",  json }
    });
}

RuleUpdatedEvent::RuleUpdatedEvent(std::shared_ptr<Rule> rule, const nlohmann::json& before)
{
    setSender(rule->getId(), "rule");
    setData({
        { "sender_full", rule->getName().Value },
        { "device", rule->getDevice().Value },
        { "action", "updated"}
    });

    nlohmann::json json;
    rule->json(json);

    nlohmann::json res;
    for(auto i : json.items()) {
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

RuleDeletedEvent::RuleDeletedEvent(std::shared_ptr<Rule> rule)
{
    setSender(rule->getId(), "rule");
    setData({
        { "sender_full", rule->getName().Value },
        { "device", rule->getDevice().Value },
        { "action", "deleted"}
    });
}

DeviceRulesChangedEvent::DeviceRulesChangedEvent(std::shared_ptr<Device> device)
{
    setSender(device->getId(), "device");
    setData({
        { "sender_full", device->getName().Value },
    });

    std::vector<nlohmann::json> rules;
    for(auto r : device->rules()) {
        nlohmann::json ruleJson;
        r->json(ruleJson);
        rules.push_back(ruleJson);
    }

    setPropertiesUpdate({
        { "rules",  rules }
    });
}

DeviceUpdatedEvent::DeviceUpdatedEvent(std::shared_ptr<Device> device, const nlohmann::json& before)
{
    setSender(device->getId(), "device");
    setData({
        { "sender_full", device->getName().Value },
        { "action", "updated"}
    });

    nlohmann::json json;
    device->json(json);

    nlohmann::json res;
    for(auto i : json.items()) {
        auto p = before.find(i.key());
        if (p != before.end()) {
           if (p.value() == i.value())
               continue;
        }
        res[i.key()] = i.value();
    }

    setPropertiesUpdate(res);
}