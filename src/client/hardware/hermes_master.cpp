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


#include "hermes_master.h"
#include <sstream>

using namespace aquabox;

HermesSerial::HermesSerial(const proto::byte_t serial[])
{
    memcpy(data(), serial, SERIAL_LEN);
}

std::string HermesSerial::to_string() const
{
    std::stringstream ss;
    for(int i = 0; i < SERIAL_LEN - 1; ++i)
    {
        ss << std::hex << (int) data()[i] << '-';
    }

    ss << std::hex << (int) data()[SERIAL_LEN -1];
    return ss.str();
}

HermesMaster::HermesMaster(std::shared_ptr<proto::MessageIO> io)
    : proto::Master(io.get())
{
}

HermesSerial HermesMaster::serial() const
{
    return HermesSerial(m_serial);
}

bool HermesMaster::isConnected() const
{
    HermesMaster* self = const_cast<HermesMaster*>(this);
    for(int i = 0; i < SERIAL_LEN; ++i) {
        if (m_serial[i] != 0) {
            return self->getPropertiesCount() > 0;
        }
    }
    return false;
}