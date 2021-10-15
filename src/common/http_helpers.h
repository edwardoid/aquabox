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

#include "iot_definitions.h"
#include "path.h"
#include <fstream>

namespace aquabox {
class HttpHelpers {
public:
    static void serveFile(Web::Response response, Path path, Web::Headers& headers, bool removeWhenFinished);
    static void serveStream(Web::Response response, Path path, std::shared_ptr<std::ifstream> ifs, size_t length, bool removeWhenFinished);
};
}
