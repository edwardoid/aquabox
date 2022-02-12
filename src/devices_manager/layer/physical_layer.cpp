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
#include "../bus/mock_gpio_bus.h"
#include "../bus/physical_1_wire_bus.h"
#include "../bus/physical_gpio_bus.h"
#include "../bus/physical_usart_bus.h"
#include <unordered_map>

using namespace aquabox;

Physical::Physical(const std::string& address)
    : Layer(address)
{}

Physical::~Physical() {}

std::shared_ptr<Bus> Physical::bus(const std::string& bus)
{
    static std::unordered_map<std::string, std::shared_ptr<Bus>> availableBuses(
        {{"GPIO", std::make_shared<PhysicalGPIOBus>(m_address + ":GPIO")},
         {"MOCK_GPIO", std::make_shared<MockGPIOBus>(m_address + ":MOCK_GPIO")},
         {"1W", std::make_shared<PhysicalOneWireBus>(m_address + ":1W")},
         {"SERIAL", std::make_shared<PhysicalUSARTBus>(m_address + ":SERIAL")}});

    auto p = availableBuses.find(bus);
    return p == availableBuses.end() ? nullptr : p->second;
}