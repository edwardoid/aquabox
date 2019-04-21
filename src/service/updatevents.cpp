#include "updatevents.h"
#include "device.h"
#include "rule.h"
#include "enumerations.h"

using namespace aquabox;

OnOffChangedEvent::OnOffChangedEvent(std::shared_ptr<Device> device, std::string reason)
{
    setSender(device->getId().Value, "device");
    setData({
        { "reason", reason },
        { "sender_full", device->getName().Value },
        { "action", Serializable<ActionType>::convert(device->turnedOn() ?  ActionType::TurnOff : ActionType::TurnOn) }
    });

    setPropertiesUpdate({
        { "isOn", device->turnedOn() }
    });
}

RuleTriggeredEvent::RuleTriggeredEvent(std::shared_ptr<Device> device)
    : OnOffChangedEvent(device, "rule")
{
}

ManualStateChangedEvent::ManualStateChangedEvent(std::shared_ptr<Device> device)
    : OnOffChangedEvent(device, "manual")
{
}

RuleEnableChangedEvent::RuleEnableChangedEvent(std::shared_ptr<Rule> rule)
{
    setSender(rule->getId().Value, "rule");
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
    setSender(rule->getId().Value, "rule");
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

RuleUpdatedEvent::RuleUpdatedEvent(std::shared_ptr<Rule> rule)
{
    setSender(rule->getId().Value, "rule");
    setData({
        { "sender_full", rule->getName().Value },
        { "device", rule->getDevice().Value },
        { "action", "updated"}
    });

    nlohmann::json json;
    rule->json(json);
    setPropertiesUpdate({
        { "*",  json }
    });
}

RuleDeletedEvent::RuleDeletedEvent(std::shared_ptr<Rule> rule)
{
    setSender(rule->getId().Value, "rule");
    setData({
        { "sender_full", rule->getName().Value },
        { "device", rule->getDevice().Value },
        { "action", "deleted"}
    });
}

DeviceRulesChangedEvent::DeviceRulesChangedEvent(std::shared_ptr<Device> device)
{
    setSender(device->getId().Value, "device");
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