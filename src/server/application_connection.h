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

namespace aquabox {
class Box;
class ApplicationConnection {
public:
    ApplicationConnection(Box& box, std::shared_ptr<Web::WSConnection> connection);
    ~ApplicationConnection();
    bool operator==(const std::shared_ptr<Web::WSConnection> connection) const;

private:
    int m_updateConnection = -1;
    Box& m_box;
    std::shared_ptr<Web::WSConnection> m_connection;
};
}
