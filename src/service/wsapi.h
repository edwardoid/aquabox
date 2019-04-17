#ifndef WSAPI_H
#define WSAPI_H

#include "log.h"
#include "route.h"

#include <server_ws.hpp>

#define DEFINE_WS_API_IMPL(Name) void API:: Name ## Impl (const Route& route, Connection& connection, Message& response)

#define ENABLE_WS_API(Name) void Name (WsServer& server, Route& route) {                                                        \
                auto& endpoint = server.endpoint[route.url()];                                                                  \
                endpoint.on_open = [this](Connection connection) {                                                              \
                    LOG_DEBUG << "Opened connection from " << connection->remote_endpoint_address(); this->onOpen(connection);  \
                };                                                                                                              \
                endpoint.on_close = [this](Connection connection, Status status, const std::string & reason) {                  \
                    LOG_DEBUG << "Connection " << connection->remote_endpoint_address()                                         \
                              << " Status: " << status << " Reason: " << reason;                                                \
                    this->onClose(connection, status, reason);                                                                  \
                };                                                                                                              \
                endpoint.on_error = [](Connection connection, const Error &ec) {                                                \
                    LOG_DEBUG << "Error occured in connection with " << connection->remote_endpoint_address() << " "            \
                              << "Error code: " << ec.category().name() << ":" << ec.value() << " " << ec.message();            \
                };                                                                                                              \
                endpoint.on_message = [&route](Connection connection, Message message) {                                        \
                    LOG_DEBUG << "Got message " << message->string() << " from " << connection->remote_endpoint_address();      \
                    API:: Name ## Impl (route, connection, message);                                                            \
                };                                                                                                              \
            }                                                                                                                   \
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
            void onOpen(Connection connection);
            void onClose(Connection connection, Status status, const std::string& reason);
            void onError(Connection connection, Error error) {}
            
            void postEvent(Event);

            ENABLE_WS_API(Ping)
        private:
            std::mutex m_lock;
            std::vector<Connection> m_connections;
        };
    }
}

#endif // WSAPI_H