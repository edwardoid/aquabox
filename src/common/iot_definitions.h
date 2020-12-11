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

#define STREAM_CONNECTION_REJECTED 0xDEADBEEF

#include <server_http.hpp>
#include <server_ws.hpp>

namespace aquabox
{
    namespace Web {
        using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
        using WSServer = SimpleWeb::SocketServer<SimpleWeb::WS>;
        using Request = std::shared_ptr<HttpServer::Request>; 
        using Response = std::shared_ptr<HttpServer::Response>;
        using Headers = SimpleWeb::CaseInsensitiveMultimap;
        using WSConnection = WSServer::Connection;
        using Message = WSServer::InMessage;
    }
}
