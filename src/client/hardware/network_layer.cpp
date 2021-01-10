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

#include "network_layer.h"
#include "mqtt_bus.h"
#include "socket_bus.h"
#include <unordered_map>

using namespace aquabox;

std::shared_ptr<Bus> NetworkLayer::bus(const std::string& bus)
{
    static std::unordered_map<std::string, std::shared_ptr<Bus> > buses({
        { "MQTT", std::make_shared<Mqtt>() },
        { "SCT", std::make_shared<SocketBus>() }
    });

    return buses.find(bus) == buses.end() ? nullptr : buses[bus];
}
