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

#include <string>
#include <rpc/types.h>
#include <types/value_change.h>
#include <vector>

namespace aquabox
{
    class IDevicesManager
    {
    public:
        virtual bool update(const DeviceRef& device) = 0;
        virtual DeviceRef get(const std::string& id) = 0;
        virtual bool set(const std::string& id, const ValueChange& value) = 0;
        virtual bool set(const std::string& id, const std::string& property, const nlohmann::json& value) = 0;
        virtual bool has(const std::string& id) = 0;
        virtual bool rulesEnabled(const std::string& id) = 0;
        virtual std::vector<DeviceRef> all() = 0;
        virtual std::vector<std::string> properties(const std::string& id) = 0;
        virtual nlohmann::json value(const std::string& id, const std::string& property) = 0;
    };
} // namespace aquabox