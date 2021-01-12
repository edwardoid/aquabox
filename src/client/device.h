/*
 * AquaBox
 * Copyright (C) 2020  Eduard Sargsyan
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

#pragma once

#include <nlohmann/json.hpp>
#include "device_type.h"
#include "action_type.h"
#include <string>
#include <set>

namespace aquabox
{
    using Metadata = nlohmann::json::object_t;

    class Device
    {
    public:
        Device() = default;
        virtual ~Device() = default;

        bool parseFile(const std::string& file);
        bool saveToFile(const std::string& file);

        std::vector<std::shared_ptr<class Rule>> rules() const;

        const std::string& id() const;
        void setId(const std::string& id);

        const std::string& name() const;
        void setName(const std::string name);

        const int32_t& index() const;
        void setIndex(const int32_t& index);

        const std::string& address() const;
        void setAddress(const std::string address);

        bool rulesEnabled() const;
        void setRulesEnabled(bool enabled);

        DeviceType type() const;
        void setType(const DeviceType& type);

        DeviceClass deviceClass() const;
        void setDeviceClass(DeviceClass deviceClass); 

        const Metadata& metadata() const;
        void setMetadata(const Metadata& metadata);

        bool supports(DeviceClass _class) const;
    protected:
        void enableSupport(DeviceClass _class);
    protected:
        std::string m_id;
        std::string m_name;
        int32_t m_index = -1;
        std::string m_address;
        bool m_rulesEnabled = true;
        DeviceType m_type = DeviceType::Generic;
        DeviceClass m_deviceClass = DeviceClass::Unknown;
        Metadata m_metadata;
        std::set<DeviceClass> m_implementation;
    };

    void to_json(nlohmann::json& j, const Device& d);
    void from_json(const nlohmann::json& j, Device& d);

    struct AbstractRelay: public Device
    {
        AbstractRelay();
        virtual bool turnedOn() const = 0;

        virtual void turnOn() = 0;
        virtual void turnOff() = 0;

        virtual void addPendingAction(uint64_t priority, ActionType action) = 0;
        virtual bool performPendingAction() = 0;
    };

    struct AbstractSensor: public Device
    {
        AbstractSensor();
        virtual double value() const = 0;
    };

    struct AbstractFeeder: public Device
    {
        virtual void feed(uint8_t times = 1) = 0;
    };
}
