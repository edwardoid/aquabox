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

HermesSerial::HermesSerial()
{
    memset(data(), 0, SERIAL_LEN);
}

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


bool HermesSerial::isValid() const
{
    for(int i = 0; i < SERIAL_LEN; ++i)
    {
        if(at(i) != 0) {
            return true;
        }
    }

    return false;
}

HermesMaster::HermesMaster(std::shared_ptr<proto::MessageIO> io)
    : proto::Master(io.get())
    , m_io(io)
{
}

HermesSerial HermesMaster::serial() const
{
    return HermesSerial(m_serial);
}

void HermesMaster::setSerial(const HermesSerial& newSerial)
{
    memcpy(m_serial, newSerial.data(), SERIAL_LEN);
}

bool HermesMaster::isConnected() const
{
    if (!m_io->good()) {
        return false;
    }
    HermesMaster* self = const_cast<HermesMaster*>(this);
    for(int i = 0; i < SERIAL_LEN; ++i) {
        if (m_serial[i] != 0) {
            return self->getPropertiesCount() > 0;
        }
    }
    return false;
}


bool HermesMaster::getPropertyData(const char *property, proto::GetValueData &value)
{
    if (m_caching)
    {
        auto pos = m_properties.find(property);
                       
        if(pos == m_properties.end() ||
           (std::chrono::system_clock::now() - pos->second.lastAccess > std::chrono::seconds(1)))
        {
            if(!proto::Master::getPropertyData(property, value)) {
                return false;
            }
            m_properties[property].lastAccess = std::chrono::system_clock::now();
            m_properties[property].value = value;
        }
        value = m_properties[property].value;
        return true;
    }

    return proto::Master::getPropertyData(property, value);
}

bool HermesMaster::setPropertyData(const char *property, const proto::SetValueData &value)
{
    if(!proto::Master::setPropertyData(property, value)) {
        m_properties.erase(property);
        return false;
    }

    if (m_caching)
    {
        auto pos = m_properties.find(property);
                       
        if(pos != m_properties.end())
        {
            pos->second.value = static_cast<const proto::ValueData>(value);
        }
    }
    return true;
}

template <>
bool proto::Master::getProperty<std::string>(const char *property, std::string &value)
{
    char raw[32];
    memset(raw, '\0', 32);
    const char* p = raw;
    if(getProperty(property, p)) {
        value = raw;
        return true;
    }
    return false;
}

template <>
bool proto::Master::setProperty<std::string>(const char *property, const std::string &value)
{
    char* raw = const_cast<char*>(value.c_str());
    return setProperty<const char*>(property, value.c_str());
}

