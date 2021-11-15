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

#include "sensor.h"
#include "hardware.h"
#include <common/log.h>

using namespace aquabox;

nlohmann::json Sensor::value(std::string prop) const
{
    if (prop != SENSOR_PROPERTY) {
        return 0.;
    }
    auto hw = Hardware::resolve(m_address);
    if (hw == nullptr) {
        LOG_ERROR << "Can't find device " << m_address;
        return 0.;
    }

    auto impl = hw->implementation<FloatValueDevice>();

    if (impl == nullptr) {
        return 0.;
    }

    return impl->get();
}