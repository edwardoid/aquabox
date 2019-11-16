#ifndef BOX_CONNECTION_H
#define BOX_CONNECTION_H

#include <memory>
#include <string>
#include <server_ws.hpp>
#include <unordered_map>

namespace aquabox
{
    class BoxConnection
    {
    public:
        using WSServer = SimpleWeb::SocketServer<SimpleWeb::WS>;
        bool empty() const { return false; }
        std::shared_ptr<WSServer::Connection> logs;
        std::shared_ptr<WSServer::Connection> events;
        std::shared_ptr<WSServer::Connection> commands;
    };
}

#endif // BOX_CONNECTION_H