/*
 * AquaBox
 * Copyright (C) 2019-2020 Eduard Sargsyan
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

#include "physical_1_wire_bus.h"
#include "one_wire_device.h"
#include "log.h"

using namespace aquabox;

#include <ow_bus.h>
#include <unordered_map>

PhysicalOneWireBus::PhysicalOneWireBus(const std::string& address)
    : Bus(address)
{}

PhysicalOneWireBus::~PhysicalOneWireBus()
{}

std::shared_ptr<Hardware> PhysicalOneWireBus::hw(const std::string& hw)
{
    static std::unordered_map<std::string, std::shared_ptr<OneWireDevice> > devices;
    if (devices.empty()) {
        for(auto& d : w1::Bus::devices()) {
            devices.insert({ d.address(), std::make_shared<OneWireDevice>(d) });
        }
    }

    auto pos = devices.find(hw);
    if (pos == devices.end()) {
        static int queriesCount;
        LOG_ERROR_IF((++queriesCount) % 10 == 0) << "Can't find 1-Wire device with adress: " << hw;
    }
    return pos == devices.end() ? nullptr : pos->second;
}