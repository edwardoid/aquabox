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
#include <log.h>
#include <event.h>
#include "hardware/hardware.h"
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

std::vector<std::shared_ptr<Rule>> Device::rules() const
{
    RulesMap& allRules = Singleton<AquaBox>::get()->rules();

    std::vector<std::shared_ptr<Rule>> res;

    allRules.for_each([&res, this](auto id, auto rule) {
        if (rule->getDevice() == m_Id) {
            res.push_back(rule);
        }
    });

    return res;
}

void Device::enableSupport(DeviceClass _class)
{
    m_implementation.insert(_class);
}

bool Device::supports(DeviceClass _class) const
{
    return m_implementation.find(_class) != m_implementation.end();
}

AbstractRelay::AbstractRelay()
{
    enableSupport(DeviceClass::Relay);
}

AbstractSensor::AbstractSensor()
{
    enableSupport(DeviceClass::Sensor);
}
