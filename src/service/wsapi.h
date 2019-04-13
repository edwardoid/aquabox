#ifndef WSAPI_H
#define WSAPI_H

#include "log.h"
#include "route.h"

#include <server_ws.hpp>

#define DEFINE_WS_API_IMPL(Name) void API:: Name ## Impl (const Route& route, Connection& connection, Message& response)

#define ENABLE_WS_API(Name) static void Name (WsServer& server, Route& route) {                                                 \
                auto& endpoint = server.endpoint[route.url()];                                                                  \
                endpoint.on_open = [](Connection connection) {                                                                  \
                    LOG_DEBUG << "Opened connection from " << connection->remote_endpoint_address(); API::onOpen(connection);   \
                };                                                                                                              \
                endpoint.on_close = [](Connection connection, Status status, const std::string & reason) {                      \
                    LOG_DEBUG << "Connection " << connection->remote_endpoint_address()                                         \
                              << " Status: " << status << " Reason: " << reason;                                                \
                    API::onClose(connection, status, reason);                                                                   \
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
    namespace ws
    {
        using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;
        using Connection = std::shared_ptr<WsServer::Connection>; 
        using Message = std::shared_ptr<WsServer::InMessage>;
        using Error = SimpleWeb::error_code;
        using Status = int;

        struct API
        {
            static void onOpen(Connection connection) {}
            static void onClose(Connection connection, Status status, const std::string& reason) {}
            static void onError(Connection connection, Error error) {}

            ENABLE_WS_API(Ping)
        };
    }
}

#endif // WSAPI_H