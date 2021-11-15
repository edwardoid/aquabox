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

#pragma once

#include "../bus/bus.h"
#include <memory>
#include <string>

namespace aquabox {
class Layer {
public:
    Layer(const std::string& address);
    virtual ~Layer();
    static std::shared_ptr<Layer> get(const std::string& layer);
    virtual std::shared_ptr<Bus> bus(const std::string& bus) = 0;

protected:
    const std::string m_address;
};
}
