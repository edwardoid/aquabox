/*
 * AquaBox
 * Copyright (C) 2023  Eduard Sargsyan
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

#include <boost/filesystem.hpp>
#include <nlohmann/json.hpp>
#include <set>
#include <string>
#include <types/device_type.h>
#include <types/value_change.h>

#define ON_OFF_PROPERTY "on"

#define SENSOR_PROPERTY "measurement"

#define STEP_PROPERTY "times"

namespace aquabox
{
    using Metadata = nlohmann::json::object_t;

    class Device
    {
    public:
        Device() = default;
        virtual ~Device() = default;

        bool parseFile(const boost::filesystem::path& file);
        nlohmann::json savedProperties(const boost::filesystem::path& file) const;
        bool saveToFile(const boost::filesystem::path& file);

        std::vector<std::shared_ptr<class Rule>> rules() const;

        const std::string& id() const;
        void setId(const std::string& id);

        const std::string& name() const;
        void setName(const std::string& name);

        const int32_t& index() const;
        void setIndex(const int32_t& index);

        const std::string& address() const;
        void setAddress(const std::string& address);

        bool rulesEnabled() const;
        void setRulesEnabled(bool enabled);

        bool accepted() const;
        void accept(bool accept);

        bool available() const;
        bool setAvailable(bool available);

        DeviceType type() const;
        void setType(const DeviceType& type);

        DeviceClass deviceClass() const;
        void setDeviceClass(DeviceClass deviceClass);

        const Metadata& metadata() const;
        void setMetadata(const Metadata& metadata);

        virtual std::vector<std::string> properties() const { return {}; }
        virtual nlohmann::json value(const std::string& property) const { return nlohmann::json(); };
        virtual bool setValue(const std::string& property, const nlohmann::json& value) { return false; };

        bool supports(DeviceClass _class) const;

        virtual void restoreLastState() {}

    protected:
        void enableSupport(DeviceClass _class);

    protected:
        std::string m_id;
        std::string m_name;
        int32_t m_index = -1;
        std::string m_address;
        bool m_rulesEnabled = true;
        bool m_accepted = true;
        bool m_available = true;
        DeviceType m_type = DeviceType::Generic;
        DeviceClass m_deviceClass = DeviceClass::Unknown;
        Metadata m_metadata;
        std::set<DeviceClass> m_implementation;
    };

    void to_json(nlohmann::json& j, const Device& d);
    void from_json(const nlohmann::json& j, Device& d);

    struct ValueChangableDevice : public Device
    {
        virtual void addPendingAction(uint64_t priority, ValueChange action) = 0;
        virtual bool performPendingAction(ValueChange& action) = 0;
    };

    struct AbstractRelay : public ValueChangableDevice
    {
        AbstractRelay();
        virtual std::vector<std::string> properties() const override { return {ON_OFF_PROPERTY}; }
        virtual void restoreLastState() override { setValue(ON_OFF_PROPERTY, value(ON_OFF_PROPERTY)); }
    };

    struct AbstractSensor : public Device
    {
        AbstractSensor();
        virtual std::vector<std::string> properties() const override { return {SENSOR_PROPERTY}; }
    };

    struct AbstractStep : public ValueChangableDevice
    {
        AbstractStep();
        virtual int steps() const = 0;
        virtual void makeSteps(int times = 1) = 0;
        virtual std::vector<std::string> properties() const override { return {STEP_PROPERTY}; }
        virtual void restoreLastState() override { setValue(STEP_PROPERTY, value(STEP_PROPERTY)); }
    };
} // namespace aquabox
