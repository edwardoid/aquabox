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

#include "physical_layer.h"
#include "physical_gpio_bus.h"
#include "mock_gpio_bus.h"
#include "physical_1_wire_bus.h"
#include "physical_usart_bus.h"
#include <unordered_map>

using namespace aquabox;

std::shared_ptr<Bus> Physical::bus(const std::string& bus)
{
    static std::unordered_map<std::string, std::shared_ptr<Bus> > availableBuses({
        { "GPIO", std::make_shared<PhysicalGPIOBus>() },
        { "MOCK_GPIO", std::make_shared<MockGPIOBus>() },
        { "1W", std::make_shared<PhysicalOneWireBus>() },
        { "SERIAL", std::make_shared<PhysicalUSARTBus>() }
    });

    auto p = availableBuses.find(bus);
    return p == availableBuses.end() ? nullptr : p->second;
}