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

#include "hardware.h"
#include "bus/bus.h"
#include "layer/layer.h"
#include <common/log.h>

using namespace aquabox;

std::shared_ptr<Hardware> Hardware::resolve(const std::string& address)
{
    auto l = address.find_first_of(':');
    auto layer = Layer::get(address.substr(0, l++));
    if (layer == nullptr) {
        LOG_ERROR << "Can't find layer: " << address.substr(0, l - 1);
        return nullptr;
    }

    auto b = address.find_first_of(':', l);
    auto bus = layer->bus(address.substr(l, b - l));
    if (bus == nullptr) {
        LOG_ERROR << "Can't find bus: " << address.substr(l, b - l);
        return nullptr;
    }

    return bus->hw(address.substr(b + 1));
}