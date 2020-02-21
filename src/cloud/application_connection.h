#ifndef APPLICATION_CONNECTION_H
#define APPLICATION_CONNECTION_H

#include <server_ws.hpp>

namespace aquabox
{
    class Box;
    class ApplicationConnection
    {
        using WSServer = SimpleWeb::SocketServer<SimpleWeb::WS>;
    private:
        ApplicationConnection(Box& box, std::shared_ptr<WSServer::Connection> connection);
        ~ApplicationConnection();
    private:
        int m_updateConnection = -1;
        Box& m_box;
        std::shared_ptr<WSServer::Connection> m_connection;
    };
}

#endif // APPLICATION_CONNECTION_H