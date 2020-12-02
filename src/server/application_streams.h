#ifndef APPLICATION_STREAMS_H
#define APPLICATION_STREAMS_H

#include <server_ws.hpp>
#include <unordered_map>
#include <mutex>
#include <memory>

namespace aquabox
{
    class ApplicationStreams: public std::mutex
    {
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

#endif // APPLICATION_STREAMS_H