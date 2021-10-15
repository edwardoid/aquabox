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

#include "validation_error_handler.h"
#include <sstream>

using namespace aquabox;

void ValidationErrorHandler::error(
    const nlohmann::json_pointer<nlohmann::basic_json<>>& pointer,
    const nlohmann::json& instance, const std::string& message)
{
    std::stringstream ss;
    nlohmann::json_schema::basic_error_handler::error(pointer, instance, message);
    ss << "ERROR: '" << pointer << "' - '" << instance << "': " << message;
    m_error = ss.str();
}

const std::string& ValidationErrorHandler::error() const { return m_error; }