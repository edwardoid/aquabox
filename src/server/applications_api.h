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
#include "api_calls_handler.h"
#include "application_connection.h"
#include <types/error_response.h>
#include <set>

namespace aquabox
{
    class ApplicationsApi: public ApiCallsHandler
    {
    public:
        void start();
    public:
        virtual bool handle(const std::string&, const CommandResult&) override;
        virtual bool handle(const std::string&, const Event&) override;
        virtual bool handle(const std::string&, const ErrorResponse&, uint64_t) override;
    private:
        void startWebServer();
        void startStreamServer();
        void removeConnection(std::shared_ptr<WSServer::Connection>  connection);
    private:
        void onOpen(std::shared_ptr<WSServer::Connection> connection);
        void onMessage( std::shared_ptr<WSServer::Connection> connection,
                        std::shared_ptr<WSServer::InMessage> message);
        void onClose(   std::shared_ptr<WSServer::Connection> connection,
                        int status, const std::string& reason);
        void onError(std::shared_ptr<WSServer::Connection> connection, SimpleWeb::error_code);
        void createConnection(std::shared_ptr<WSServer::Connection> ws);
        void sendError(std::shared_ptr<WSServer::Connection> ws, const ErrorResponse& error);
    private:
        Route m_base;
        Route m_rpc;
        std::thread m_httpRoutine;
        std::thread m_streamRoutine;
        std::shared_ptr<HttpServer> m_http;
        std::shared_ptr<WSServer> m_ws;
        std::map<ApplicationConnectionRawPtr, std::shared_ptr<ApplicationConnection>> m_connections;
        v1::REST RESTApi;
    };
} // namespace aquabox
