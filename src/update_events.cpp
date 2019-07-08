#include "update_events.h"
#include "relay.h"
#include "sensor.h"
#include "rule.h"
#include "enumerations.h"

using namespace aquabox;

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
           if (p.value() == i.value())
               continue;
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