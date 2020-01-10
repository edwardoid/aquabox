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

#include "device.h"
#include "aquabox.h"
#include "rule.h"
#include "log.h"
#include "event.h"
#include "hardware.h"
#include <time.h>
#include <fstream>

using namespace aquabox;

Device::Device()
    : ClassProperty("-class Device", { &m_Id, &m_Index, &m_Address, &m_RulesEnabled, &m_Name, &m_Type, &m_Class, &m_Meta })
    , m_Id("id"), m_Index("index", false), m_Address("address"), m_RulesEnabled("rulesEnabled")
    , m_Name("name"), m_Type("type"), m_Class("class"), m_Meta("meta")
{}

Device::Device(const Device& src)
    : Device()
{
    *this = src;
}

bool Device::parseFile(const std::string& file)
{
    std::ifstream is(file);

    std::string content, line;
    while(std::getline(is, line)) {
        content += line;
    }

    nlohmann::json root = nlohmann::json::parse(content);

    if(!parse(root)) {
        LOG_ERROR << "Device from " << file << " can not be parsed!";
        return false; 
    }
    return true;
}

bool Device::saveToFile(const std::string& file)
{
    std::ofstream os(file);
    if(!os) {
        LOG_ERROR << "Can't write to " << file << strerror(errno);
        return false;
    }
    nlohmann::json data;
    json(data);
    os << data.dump(4);
    os.flush();
    os.close();
    return true;
}

Device::Device(Device&& src)
    : Device()
{
    moveData(src);
    moveProperty(Id);
    moveProperty(Index);
    moveProperty(Address);
    moveProperty(RulesEnabled);
    moveProperty(Name);
    moveProperty(Type);
    moveProperty(Class);
    moveProperty(Meta);
}

const Device& Device::operator = (const Device& src)
{
    copyData(src);
    copyProperty(Id);
    copyProperty(Index);
    copyProperty(Address);
    copyProperty(RulesEnabled);
    copyProperty(Name);
    copyProperty(Type);
    copyProperty(Class);
    copyProperty(Meta);

    return *this;
}

void Device::copyData(const Device& src)
{
    copyProperty(Index);
    copyProperty(RulesEnabled);
    copyProperty(Name);
    copyProperty(Type);
    copyProperty(Class);
    copyProperty(Meta);
}

Bytes Device::read(uint32_t len) const
{
    Bytes data(len, 0);
    auto hw = Hardware::resolve(m_Address);
    if (hw == nullptr) {
        return Bytes();
    }

    
    auto got = hw->read(data);
    data.resize(static_cast<uint32_t>(got));
    return data;
}

bool Device::isRelay() const
{
    return getClass() == DeviceClass::Relay;
}

bool Device::isSensor() const
{
    return getClass() == DeviceClass::Sensor;
}

std::vector<std::shared_ptr<Rule>> Device::rules() const
{
    RulesMap rules = AquaBox::i().rules();
    std::vector<std::shared_ptr<Rule>> res;
    for(auto r : rules) {
        if (r.second->getDevice() == m_Id) {
            res.push_back(r.second);
        }
    }

    return res;
}
