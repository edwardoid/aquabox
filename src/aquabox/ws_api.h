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

#ifndef WSAPI_H
#define WSAPI_H

#include "log.h"
#include "route.h"
#include <server_ws.hpp>

#define DEFINE_WS_API_IMPL(Name) void API:: Name ## Impl (const Route& route, Connection& connection, Message& response)
                                 

#define ENABLE_WS_API(Name)                                                                                                                                 \
                std::string Name ## Url;                                                                                                                    \
                void Name (WsServer& server, Route& route) {                                                                                                \
                Name ## Url = route.url();                                                                                                                  \
                auto& endpoint = server.endpoint[route.url()];                                                                                              \
                endpoint.on_open = [this](Connection connection) {                                                                                          \
                    LOG_DEBUG << "Opened connection from " << connection->remote_endpoint().address().to_string(); this->onOpen(Name ## Url, connection);   \
                };                                                                                                                                          \
                endpoint.on_close = [this](Connection connection, Status status, const std::string & reason) {                                              \
                    LOG_DEBUG << "Connection " << connection->remote_endpoint().address().to_string()                                                       \
                              << " Status: " << status << " Reason: " << reason;                                                                            \
                    this->onClose(Name ## Url, connection, status, reason);                                                                                 \
                };                                                                                                                                          \
                endpoint.on_error = [](Connection connection, const Error &ec) {                                                                            \
                    LOG_DEBUG << "Error occured in connection with " << connection->remote_endpoint().address().to_string() << " "                          \
                              << "Error code: " << ec.category().name() << ":" << ec.value() << " " << ec.message();                                        \
                };                                                                                                                                          \
                endpoint.on_message = [&route](Connection connection, Message message) {                                                                    \
                    LOG_DEBUG << "Got message " << message->string() << " from " << connection->remote_endpoint().address().to_string();                    \
                    API:: Name ## Impl (route, connection, message);                                                                                        \
                };                                                                                                                                          \
            }                                                                                                                                               \
            static void Name ## Impl (const Route& route, Connection& connection, Message& response);

namespace aquabox
{
    class Event;
    namespace ws
    {
        using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;
        using Connection = std::shared_ptr<WsServer::Connection>; 
        using Message = std::shared_ptr<WsServer::InMessage>;
        using Error = SimpleWeb::error_code;
        using Event = std::shared_ptr<aquabox::Event>;
        using Status = int;

        struct API
        {
            void onOpen(const std::string& route, Connection connection);
            void onClose(const std::string& route, Connection connection, Status status, const std::string& reason);
            void onError(const std::string& route, Connection connection, Error error) {}
            
            void postEvent(Event);

            ENABLE_WS_API(Updates);
            ENABLE_WS_API(Commands);
        private:
            std::mutex m_lock;
            using ConnectionsList = std::vector<Connection>;
            std::unordered_map<std::string /* route */, ConnectionsList> m_connections;
        };
    }
}

#endif // WSAPI_H