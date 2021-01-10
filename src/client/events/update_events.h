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

#pragma once

#include <event.h>

namespace aquabox
{
    class OnOffChangedEvent: public Event
    {
    public:
        OnOffChangedEvent(std::shared_ptr<class AbstractRelay> device, std::string reason);
    };

    class SensorChangedEvent: public Event
    {
    public:
        SensorChangedEvent(std::shared_ptr<class AbstractSensor> sensor, std::string reason);
    };

    class RuleTriggeredEvent: public OnOffChangedEvent
    {
    public:
        RuleTriggeredEvent(std::shared_ptr<class AbstractRelay> device);
    };

    class ManualStateChangedEvent: public OnOffChangedEvent
    {
    public:
        ManualStateChangedEvent(std::shared_ptr<class AbstractRelay> device);
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
        RuleUpdatedEvent(std::shared_ptr<class Rule> rule, const nlohmann::json& before);
    };

    class DeviceRulesChangedEvent: public Event
    {
    public:
        DeviceRulesChangedEvent(std::shared_ptr<class Device> device);
    };

    class DeviceUpdatedEvent: public Event
    {
    public:
        DeviceUpdatedEvent(std::shared_ptr<class Device> device, const nlohmann::json& before);
    };
}
