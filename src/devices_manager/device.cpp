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
#include "../common/enumerations.h"
#include "../common/log.h"
#include "../types/rule.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <rpc/converters.h>
#include <time.h>
#include <types.grpc.pb.h>

namespace aquabox
{
    void to_json(nlohmann::json& j, const Device& d)
    {
        try {
            j = nlohmann::json{{"id", d.id()},
                               {"name", d.name()},
                               {"index", d.index()},
                               {"address", d.address()},
                               {"rules_enabled", d.rulesEnabled()},
                               {"type", Serializable<DeviceType>::convert(d.type())},
                               {"class", Serializable<DeviceClass>::convert(d.deviceClass())},
                               {"meta", d.metadata()},
                               {"accepted", d.accepted()},
                               {"available", d.available()}};
            nlohmann::json props;
            for(auto p : d.properties())
            {
                props[p] = d.value(p);
            }
            j["properties"] = props;
        }
        catch (const nlohmann::detail::exception& e) {
            LOG_ERROR << "Can't serialize Device: " << e.what();
        }
    }

    void from_json(const nlohmann::json& j, Device& d)
    {
        try {
            d.setId(j.at("id"));
            d.setName(j.at("name"));
            d.setIndex(j.at("index"));
            d.setAddress(j.at("address"));
            d.setRulesEnabled(j.at("rules_enabled"));
            d.setType(Serializable<DeviceType>::convert((std::string) j.at("type")));
            d.setDeviceClass(Serializable<DeviceClass>::convert((std::string) j.at("class")));
            d.setMetadata(j.at("meta"));
            d.accept(j.at("accepted"));

            // Can't restore properties here!
        }
        catch (std::exception& e) {
            LOG_ERROR << "Parsing failed: " << e.what();
        }
        catch (...) {
            LOG_ERROR << "Parsing failed";
        }
    }

    //  --------------
    template<>
    Device services::Converter<services::DeviceData, Device>::convert(const services::DeviceData& data)
    {
        Device device;
        device.setId(data.id());
        device.setName(data.name());
        device.setAddress(data.address());
        device.setRulesEnabled(data.rulesenabled());
        device.setAvailable(data.available());
        device.accept(data.accepted());
        device.setType(Serializable<DeviceType>::convert(data.type()));
        device.setDeviceClass(Serializable<DeviceClass>::convert(data.clazz()));
        Metadata meta;
        for (const auto& m : data.meta()) {
            meta[m.first] = m.second;
        }
        device.setMetadata(meta);
        return device;
    }

    template<>
    services::DeviceData services::Converter<Device, ::aquabox::services::DeviceData>::convert(const Device& data)
    {
        services::DeviceData device;
        device.set_id(data.id());
        device.set_name(data.name());
        device.set_address(data.address());
        device.set_rulesenabled(data.rulesEnabled());
        device.set_available(data.available());
        device.set_accepted(data.accepted());
        device.set_type(Serializable<DeviceType>::convert(data.type()));
        device.set_clazz(Serializable<DeviceClass>::convert(data.deviceClass()));
        for (auto& m : data.metadata()) {
            device.mutable_meta()->insert({m.first, (std::string) m.second});
        }
        return device;
    }

} // namespace aquabox

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

bool Device::accepted() const
{
    return m_accepted;
}

void Device::accept(bool a)
{
    m_accepted = a;
}

bool Device::available() const
{
    return m_available;
}

bool Device::setAvailable(bool available)
{
    return m_available = available;
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
    enableSupport(deviceClass);
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
    /*
  RulesMap& allRules = Singleton<AquaBox>::get()->rules();

  std::vector<std::shared_ptr<Rule>> res;

  allRules.for_each([&res, this](auto id, auto rule) {
      if (rule->device() == m_id) {
          res.push_back(rule);
      }
  });

  return res;*/
    return {};
}

void Device::enableSupport(DeviceClass _class)
{
    m_implementation.insert(_class);
}

bool Device::supports(DeviceClass _class) const
{
    return m_implementation.find(_class) != m_implementation.end();
}

bool Device::parseFile(const boost::filesystem::path& file)
{
    std::ifstream ifs(file.string());
    if (!ifs)
        return false;
    nlohmann::json json;
    ifs >> json;
    *this = json;
    return true;
}

nlohmann::json Device::savedProperties(const boost::filesystem::path& file) const
{
    std::ifstream ifs(file.string());
    if (!ifs)
        return nlohmann::json{};
    nlohmann::json json;
    ifs >> json;
    return json["properties"];
}

bool Device::saveToFile(const boost::filesystem::path& file)
{
    std::ofstream ofs(file.string());
    if (!ofs)
        return false;
    nlohmann::json json;
    json = *this;
    ofs << json.dump(4);
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

AbstractStep::AbstractStep()
{
    enableSupport(DeviceClass::Step);
}
