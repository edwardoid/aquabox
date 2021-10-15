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

#include "hermes_device_builder.h"
#include "../device.h"
#include "../relay.h"
#include "../sensor.h"
#include "../step.h"
#include "hermes_hardware.h"

#include <common/enumerations.h>

using namespace aquabox;

std::shared_ptr<class Device>
HermesDeviceBuilder::build(std::string address,
    std::shared_ptr<class HermesMaster> master)
{
    if (!master->serial().isValid()) {
        LOG_ERROR << "Serial for Hermes device to build is invalid";
        return nullptr;
    }
    std::string tp;
    if (master->getProperty("type", tp)) {
        LOG_DEBUG << "Slave type: " << tp;
    } else {
        LOG_DEBUG << "Can't get property 'type'";
        return nullptr;
    }

    std::string cs;
    if (master->getProperty("class", cs)) {
        LOG_DEBUG << "Slave class: " << cs;
    } else {
        LOG_DEBUG << "Can't get property 'class'";
        return nullptr;
    }

    bool ok = false;
    auto clazz = Serializable<DeviceClass>::convert(cs, &ok);
    if (!ok) {
        LOG_ERROR << "Wrong class" << cs;
        return nullptr;
    }

    auto type = Serializable<DeviceType>::convert(tp, &ok);

    if (!ok) {
        LOG_ERROR << "Wrong type " << tp;
        return nullptr;
    }

    std::shared_ptr<Device> dev;
    switch (clazz) {
    case DeviceClass::Relay: {
        dev = std::make_shared<Relay>();
        dev->setName("Relay");
        break;
    }
    case DeviceClass::Sensor: {
        dev = std::make_shared<Sensor>();
        dev->setName("Sensor");
        break;
    }
    case DeviceClass::Step: {
        dev = std::make_shared<Step>();
        dev->setName("Step");
        break;
    }
    default: {
        LOG_ERROR << "Unknown device type in " << cs;
        return nullptr;
    }
    }

    dev->setAddress(address);
    dev->setType(type);
    dev->setDeviceClass(clazz);
    dev->setMetadata({ { "serial", master->serial().to_string() } });

    return dev;
}