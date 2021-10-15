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

#include "devices_map.h"
#include "device.h"
#include <devices_manager/relay.h>
#include <devices_manager/sensor.h>
#include <devices_manager/step.h>
#include <micro_clients/settings.h>
#include <common/log.h>

#include <fstream>

using namespace aquabox;

DeviceClass getClass(const Path& p)
{
    std::ifstream is(p.c_str());
    std::string content;
    std::string line;
    while (std::getline(is, line)) {
        content += line;
    }

    try {
        nlohmann::json data = nlohmann::json::parse(content);
        auto val = data.find("class").value();
        if (val.is_string()) {
            bool ok = false;
            auto clazz = Serializable<DeviceClass>::convert(val.get<std::string>(), &ok);
            return ok ? clazz : DeviceClass::Unknown;
        }
    } catch (std::exception e) {
        LOG_ERROR << "Can't determine device type in " << p.c_str()
                  << " error: " << e.what();
    }

    return DeviceClass::Unknown;
}

DevicesMap::DevicesMap() { }

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
    return this->has([&address](const std::shared_ptr<Device>& dev) {
        return dev->address() == address;
    });
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

bool DevicesMap::parse(Path& root)
{
    std::unordered_map<std::string, std::shared_ptr<Device>> devices;
    for (Path p : root.children()) {
        if (p.isDirectory() || p.extension() != "json") {
            continue;
        }

        LOG_INFO << "Loading device: " << p.c_str();
        std::shared_ptr<Device> dev = nullptr;
        switch (getClass(p)) {
        case DeviceClass::Relay: {
            dev = std::make_shared<Relay>();
            break;
        }
        case DeviceClass::Sensor: {
            dev = std::make_shared<Sensor>();
            break;
        }
        case DeviceClass::Step: {
            dev = std::make_shared<Step>();
            break;
        }
        default: {
            LOG_ERROR << "Unknown device type in " << p.c_str();
        }
        }

        if (dev != nullptr && dev->parseFile(p)) {
            devices.insert({ dev->id(), dev });
        }
    }
    this->set(devices);
    return true;
}

std::set<std::string> DevicesMap::ids() const { return keys(); }

std::string DevicesMap::addDevice(std::shared_ptr<Device> dev, bool pending)
{
    std::string id = dev->id();
    int index = 0;
    this->for_each(
        [&index](const std::string&, const std::shared_ptr<Device> dev) {
            if (dev->index() >= index) {
                index = dev->index() + 1;
            }
        });
    dev->setIndex(index);
    LOG_INFO << "Adding device " << id;
    insert(id, dev);
    return id;
}