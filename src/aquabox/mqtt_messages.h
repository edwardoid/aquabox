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

#ifndef MQTT_MESSAGES_H
#define MQTT_MESSAGES_H

namespace aquabox
{
    enum class Command: uint16_t
    {
        AuthRequest = 0
    };

    enum class Capability: uint8_t
    {
        Relay = 1 << 0,
        Sensor = 1 << 1
    };

    struct HarwareAddress
    {
        uint8_t a0;
        uint8_t a1;
        uint8_t a2;
        utin8_t a3;
    };

    template<typename DataT>
    struct Message
    {
        Command command;
        DataT data;
    };

    struct AuthRequest
    {
        HarwareAddress adress;
        Capability capabilities;
    }

    struct AuthResponse
    {
        uint8_t response;
    };
}