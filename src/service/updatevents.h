#ifndef UPDATE_EVENTS_H
#define UPDATE_EVENTS_H

#include "event.h"

namespace aquabox
{
    class OnOffChangedEvent: public Event
    {
    public:
        OnOffChangedEvent(std::shared_ptr<class Device> device, std::string reason);
    };

    class RuleTriggeredEvent: public OnOffChangedEvent
    {
    public:
        RuleTriggeredEvent(std::shared_ptr<class Device> device);
    };

    class ManualStateChangedEvent: public OnOffChangedEvent
    {
    public:
        ManualStateChangedEvent(std::shared_ptr<class Device> device);
    };

    class RuleEnableChangedEvent: public Event
    {
    public:
        RuleEnableChangedEvent(std::shared_ptr<class Rule> rule);
    };

    class RuleDeletedEvent: public Event
    {
    public:
        RuleDeletedEvent(std::shared_ptr<class Rule> rule);
    };

    class RuleCreatedEvent: public Event
    {
    public:
        RuleCreatedEvent(std::shared_ptr<class Rule> rule);
    };

    class RuleUpdatedEvent: public Event
    {
    public:
        RuleUpdatedEvent(std::shared_ptr<class Rule> rule);
    };

    class DeviceRulesChangedEvent: public Event
    {
    public:
        DeviceRulesChangedEvent(std::shared_ptr<class Device> device);
    };
}

#endif // UPDATE_EVENTS_H