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

#include "device_reference.h"
#include "aquabox.h"
#include "device.h"

using namespace aquabox;

DeviceReference::DeviceReference(const std::string& name, bool required)
    : StringProperty(name.c_str(), required)
{}

std::shared_ptr<Device> DeviceReference::resolve() const
{
    return Singleton<AquaBox>::get()->devices().get(Value, nullptr);
}