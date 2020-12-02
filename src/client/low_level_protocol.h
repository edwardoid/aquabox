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

#ifndef LOW_LEVEL_PROTOCOL_H
#define LOW_LEVEL_PROTOCOL_H

namespace aquabox
{
    class LowLevelProtocol
    {
        public:

        enum class Command: uint8_t

        struct Packet
        {
            uint16_t crc = 0;
            char sender[16];

            uint8_t dalaLen = 0;
            char data[256]; 
        };
        virtual bool hasPacket() const { return false; }
        virtual void sendPacket(const Packet& packet) {}
        virtual Packet getPacket() { return Packet{}; }
    };
}

#endif // LOW_LEVEL_PROTOCOL_H