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

#include "../device.h"
#include "device_events.h"
#include <types/serial.h>

using namespace aquabox;

DeviceAddedEvent::DeviceAddedEvent(std::shared_ptr<Device> device)
{
    setData({{"action", "device_added"}, {"device", device->id()}});
}

DeviceDeletedEvent::DeviceDeletedEvent(std::shared_ptr<Device> device)
{
    setData({{"action", "device_deleted"}, {"device", device->id()}});
}

DeviceUpdatedEvent::DeviceUpdatedEvent(std::shared_ptr<Device> device, const nlohmann::json& before)
{
    setSender(device->id(), "device");
    setData({{"sender_full", device->name()}, {"action", "updated"}});

    nlohmann::json json = *device;

    nlohmann::json res;
    for (auto i : json.items()) {
        auto p = before.find(i.key());
        if (p != before.end()) {
            if (p.value() == i.value())
                continue;
        }
        res[i.key()] = i.value();
    }

    setPropertiesUpdate(res);
}

DevicePropertyUpdateEvent::DevicePropertyUpdateEvent(std::shared_ptr<Device> device, const nlohmann::json& update)
{
    setSender(device->id(), "device");
    setData({{"sender_full", device->name()}, {"action", "updated"}});

    setPropertiesUpdate(update);
}

ValueChangedEvent::ValueChangedEvent(std::shared_ptr<Device> device, std::string property, const nlohmann::json& value, std::string reason)
{
    setSender(device->id(), "device");
    setData({
        {"reason", reason},
        {"sender_full", device->name()},
    });

    setPropertiesUpdate({{property, value}});
}
