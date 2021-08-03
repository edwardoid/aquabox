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
#include <stdint.h>

namespace aquabox
{
    class ISettings
    {
    public:
        virtual std::int32_t get(const std::string& key, std::int32_t defaultValue) = 0;
        virtual void set(const std::string& key, std::int32_t value) = 0;
        virtual std::string get(const std::string& key, const std::string defaultValue) = 0;
        virtual void set(const std::string&, const std::string& value) = 0;
        virtual std::uint32_t port(const std::string& service, bool request = false) = 0;
    };
}