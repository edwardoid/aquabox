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

#include "server_connection.h"
#include "server_aquabox_connection.h"

using namespace aquabox;

std::shared_ptr<CommandHandler> ServerConnection::get(const std::string& id)
{
    if(!m_openConnections.count(id)) {
        m_openConnections.insert({ id, std::make_shared<ServerAquaboxConnection>(id) });
    }

    return std::dynamic_pointer_cast<CommandHandler>(m_openConnections[id]);
}