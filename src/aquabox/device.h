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

#ifndef DEVICE_H
#define DEVICE_H

#include <nlohmann/json.hpp>
#include "device_type.h"
#include "property.h"
#include "hardware.h"
#include "action_type.h"
#include <string>
#include <set>

namespace aquabox
{
    using Metadata = PrimitiveProperty<nlohmann::json, nlohmann::detail::value_t::object>;

    class Device: public ClassProperty
    {
    public:
        Device();
        Device(const Device& src);
        Device(Device&& src);
        const Device& operator = (const Device& src);
        void copyData(const Device& src);

        bool parseFile(const std::string& file);
        bool saveToFile(const std::string& file);

        std::vector<std::shared_ptr<class Rule>> rules() const;

        //Bytes read(uint32_t len) const;

        property(Id, StringProperty)
        property(Index, UnsignedNumberProperty)
        property(Address, StringProperty);
        property(RulesEnabled, BooleanProperty)
        property(Name, StringProperty)
        property(Type, EnumProperty<DeviceType>)
        property(Class, EnumProperty<DeviceClass>)
        property(Meta, Metadata)

        bool supports(DeviceClass _class) const;
    protected:
        void enableSupport(DeviceClass _class);
    private:
        std::set<DeviceClass> m_implementation;
    };

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
}

#endif // DEVICE_H