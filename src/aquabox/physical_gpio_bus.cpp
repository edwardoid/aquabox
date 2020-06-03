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

#include "physical_gpio_bus.h"
#include "gpio.h"
#include <unordered_map>

using namespace aquabox;

std::shared_ptr<Hardware> PhysicalGPIOBus::hw(std::string hw)
{
    static std::unordered_map<std::string, std::shared_ptr<GPIO> > pins({
        { "12", nullptr },
        { "11", nullptr },
        { "6", nullptr },
        { "1", nullptr },
        { "0", nullptr },
        { "3", nullptr },
        { "4", nullptr }, // armv6
        { "15", nullptr },
        { "16", nullptr },
        { "14", nullptr },
        { "198", nullptr },
        { "199", nullptr },
        { "7", nullptr },
        { "19", nullptr },
        { "18", nullptr },
        { "2", nullptr },
        { "13", nullptr },
        { "10", nullptr }
    });

    auto pos = pins.find(hw);
    if (pos == pins.end())
        return nullptr;
    if (pos->second == nullptr)
        pos->second = std::make_shared<GPIO>(pos->first);

    return pos->second;
}