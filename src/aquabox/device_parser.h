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

#ifndef DEVICE_PARSER_H
#define DEVICE_PARSER_H

#include <unordered_map>
#include <memory>
#include "path.h"

namespace aquabox
{
    class Device;

    class DeviceParser
    {
        public:
        static std::unordered_map<std::string, std::shared_ptr<Device>> parse(Path& root);
    };
}

#endif // DEVICE_PARSER_H