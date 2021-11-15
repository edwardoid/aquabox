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

#include <memory>
#include <mutex>
#include <server_ws.hpp>
#include <unordered_map>

namespace aquabox {
class ApplicationStreams : public std::mutex {
public:
    using WSServer = SimpleWeb::SocketServer<SimpleWeb::WS>;
    using Connection = WSServer::Connection;
    bool hasConnectionToBox(const std::string& box);
    std::shared_ptr<Connection> getConnectionToBox(const std::string& box);
    void addConnectionTo(const std::string& box, std::shared_ptr<Connection> connection);
    bool removeConnectionTo(const std::string& box);

private:
    std::unordered_map<std::string, std::shared_ptr<Connection>> m_connections;
};
}
