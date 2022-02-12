/*
 * AquaBox
 * Copyright (C) 2020  Eduard Sargsyan
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

#include <types/box_event.h>

namespace aquabox
{
    class DeviceAddedEvent : public BoxEvent
    {
    public:
        DeviceAddedEvent(std::shared_ptr<class Device> device);
    };

    class DeviceDeletedEvent : public BoxEvent
    {
    public:
        DeviceDeletedEvent(std::shared_ptr<class Device> device);
    };

    class DeviceUpdatedEvent : public Event
    {
    public:
        DeviceUpdatedEvent(std::shared_ptr<class Device> device, const nlohmann::json& before);
    };

    class DevicePropertyUpdateEvent : public Event
    {
    public:
        DevicePropertyUpdateEvent(std::shared_ptr<class Device> device, const nlohmann::json& updates);
    };

    class ValueChangedEvent : public Event
    {
    public:
        ValueChangedEvent(std::shared_ptr<class Device> device, std::string property, const nlohmann::json& value, std::string reason);
    };

} // namespace aquabox
