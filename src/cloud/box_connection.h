#ifndef BOX_CONNECTION_H
#define BOX_CONNECTION_H

#include "command.h"
#include "results_map.h"
#include <memory>
#include <string>
#include <server_ws.hpp>
#include <unordered_map>

namespace aquabox
{
    class BoxConnection
    {
    public:
        BoxConnection(ResultsMap<Command>& results);
        using WSServer = SimpleWeb::SocketServer<SimpleWeb::WS>;
        bool empty() const { return false; }
        void ready(std::string data);
        void runCommand(Command& cmd);
        ResultsMap<Command>& results;
        std::shared_ptr<WSServer::Connection> logs;
        std::shared_ptr<WSServer::Connection> events;
        std::shared_ptr<WSServer::Connection> commands;
    };
}

#endif // BOX_CONNECTION_H