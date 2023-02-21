/*
 * AquaBox
 * Copyright (C) 2023  Eduard Sargsyan
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

#include "devices_map.h"
#include "device.h"
#include <common/log.h>
#include <devices_manager/relay.h>
#include <devices_manager/sensor.h>
#include <devices_manager/step.h>
#include <micro_clients/settings.h>

#include <fstream>

using namespace aquabox;

DevicesMap::DevicesMap() {}

std::string DevicesMap::nextId() const
{
    static std::string prefix = "dev_";
    std::string candidate = prefix + std::to_string(rand());
    while (has(candidate)) {
        candidate = prefix + std::to_string(rand());
    }

    return candidate;
}

bool DevicesMap::hasDevice(const std::string& address) const
{
    return this->has([&address](const std::shared_ptr<Device>& dev) { return dev->address() == address; });
}

std::string DevicesMap::deviceIdByAddress(const std::string& address) const
{
    std::string id;
    bool found = this->has([&address, &id](const std::shared_ptr<Device>& dev) {
        if (dev->address() == address) {
            id = dev->id();
            return true;
        }
        return false;
    });

    return found ? id : "";
}

std::set<std::string> DevicesMap::ids() const
{
    return keys();
}

std::string DevicesMap::addDevice(std::shared_ptr<Device> dev, bool pending)
{
    std::string id = dev->id();
    int index = 0;
    this->for_each([&index](const std::string&, const std::shared_ptr<Device> dev) {
        if (dev->index() >= index) {
            index = dev->index() + 1;
        }
    });
    dev->setIndex(index);
    LOG_INFO << "Adding device " << id;
    insert(id, dev);
    return id;
}