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

#include <types/event.h>
#include <types/types.h>

namespace aquabox {

class RuleEnableChangedEvent : public Event {
public:
    RuleEnableChangedEvent(std::shared_ptr<class Rule> rule);
};

class RuleDeletedEvent : public Event {
public:
    RuleDeletedEvent(const class Rule& rule);
};

class RuleCreatedEvent : public Event {
public:
    RuleCreatedEvent(const class Rule& rule);
};

class RuleUpdatedEvent : public Event {
public:
    RuleUpdatedEvent(const class Rule& rule, const nlohmann::json& before);
};

class TemporaryRuleRemainEvent : public Event {
public:
    TemporaryRuleRemainEvent(const class Rule& rule);
};

class DeviceRulesChangedEvent : public Event {
public:
    DeviceRulesChangedEvent(const DeviceRef& device);
};

}
