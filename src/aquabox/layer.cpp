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

#include "layer.h"
#include "physical_layer.h"
#include "network_layer.h"
#include <unordered_map>

using namespace aquabox;

std::shared_ptr<Layer> Layer::get(std::string layer)
{
    static std::unordered_map<std::string, std::shared_ptr<Layer>> availableLayers({
        { "PHY", std::make_shared<Physical>() },
        { "NET", std::make_shared<NetworkLayer>() }
    });

    auto pos = availableLayers.find(layer);
    return pos == availableLayers.end() ? nullptr : pos->second;
}