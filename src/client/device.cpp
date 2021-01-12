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
#include <nlohmann/json.hpp>

namespace aquabox {
void to_json(nlohmann::json& j, const Device& d) {
    try{
        j = nlohmann::json{
            { "id", d.id() },
            { "name", d.name() },
            { "index", d.index() },
            { "address", d.address() },
            { "rulesEnabled", d.rulesEnabled() },
            { "type", Serializable<DeviceType>::convert(d.type()) },
            { "class", Serializable<DeviceClass>::convert(d.deviceClass()) },
            { "meta", d.metadata() }
        };
    } catch(nlohmann::detail::exception e) {
        LOG_ERROR << "Can't serialize Device";
    }
}

void from_json(const nlohmann::json& j, Device& d) {
    d.setId(j.at("id"));
    d.setName(j.at("name"));
    d.setIndex(j.at("index"));
    d.setAddress(j.at("address"));
    d.setRulesEnabled(j.at("rulesEnabled"));
    d.setType(Serializable<DeviceType>::convert((std::string)j.at("type")));
    d.setDeviceClass(Serializable<DeviceClass>::convert((std::string)j.at("class")));
    d.setMetadata(j.at("meta"));
}

}

using namespace aquabox;

const std::string& Device::id() const
{
    return m_id;    
}

void Device::setId(const std::string& id)
{
    m_id = id;
}

const std::string& Device::name() const
{
    return m_name;    
}

void Device::setName(const std::string name)
{
    m_name = name;
}

const int32_t& Device::index() const
{
    return m_index;
}


void Device::setIndex(const int32_t& index)
{
    m_index = index;
}

const std::string& Device::address() const
{
    return m_address;    
}

void Device::setAddress(const std::string address)
{
    m_address = address;
}

bool Device::rulesEnabled() const
{
    return m_rulesEnabled;    
}

void Device::setRulesEnabled(bool enabled)
{
	m_rulesEnabled = enabled;
}

DeviceType Device::type() const
{
	return m_type;
}

void Device::setType(const DeviceType& type)
{
	m_type = type;
}

DeviceClass Device::deviceClass() const
{
	return m_deviceClass;
}

void Device::setDeviceClass(DeviceClass deviceClass)
{
	m_deviceClass = deviceClass;
}

const Metadata& Device::metadata() const
{
	return m_metadata;
}

void Device::setMetadata(const Metadata& metadata)
{
	m_metadata = metadata;
}

std::vector<std::shared_ptr<Rule>> Device::rules() const
{
    RulesMap& allRules = Singleton<AquaBox>::get()->rules();

    std::vector<std::shared_ptr<Rule>> res;

    allRules.for_each([&res, this](auto id, auto rule) {
        if (rule->getDevice() == m_id) {
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

bool Device::parseFile(const std::string& file)
{
    std::ifstream ifs(file);
    if(!ifs) return false;
    nlohmann::json json;
    ifs >> json;
    *this = json;
    return true;
}

bool Device::saveToFile(const std::string& file)
{
    std::ofstream ofs(file);
    if(!ofs) return false;
    nlohmann::json json;
    json = *this;
    ofs << json;
    return true;
}

AbstractRelay::AbstractRelay()
{
    enableSupport(DeviceClass::Relay);
}

AbstractSensor::AbstractSensor()
{
    enableSupport(DeviceClass::Sensor);
}
