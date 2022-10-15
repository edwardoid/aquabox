/*
 * AquaBox
 * Copyright (C) 2019-2022  Eduard Sargsyan
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
#include "rest_api.h"
#include <types/error_response.h>

namespace aquabox
{
    class ApplicationsApi
    {
    public:
        void start();

    private:
        void startWebServer();
        void startStreamServer();
        void removeConnection(const std::string& boxId, std::shared_ptr<WSServer::Connection> connection);
    private:
        void onOpen(std::shared_ptr<WSServer::Connection> connection);
        void onMessage( std::shared_ptr<WSServer::Connection> connection,
                        std::shared_ptr<WSServer::InMessage> message);
        void onClose(   std::shared_ptr<WSServer::Connection> connection,
                        int status, const std::string& reason);
        void onError(std::shared_ptr<WSServer::Connection> connection, SimpleWeb::error_code);
        void createConnection(std::shared_ptr<WSServer::Connection> ws, const std::string& boxId);
        void sendError(std::shared_ptr<WSServer::Connection> ws, const ErrorResponse& error);
    private:
        Route m_base;
        Route m_rpc;
        std::thread m_httpRoutine;
        std::thread m_streamRoutine;
        std::shared_ptr<HttpServer> m_http;
        std::shared_ptr<WSServer> m_ws;
        std::unordered_map<std::uint32_t /* id */, std::shared_ptr<WSServer::Connection>> m_connections;
        v1::REST RESTApi;
    };
} // namespace aquabox
