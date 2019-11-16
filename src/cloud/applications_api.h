#ifndef APPLICATIONS_API_H
#define APPLICATIONS_API_H

#include <server_http.hpp>
#include <server_ws.hpp>

namespace aquabox
{
    class ApplicationsApi
    {
        using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
        using WSServer = SimpleWeb::SocketServer<SimpleWeb::WS>;
    public:
        static void start();
    private:
        static void startServer();
    private:
        static ApplicationsApi m_instance;
    private:
        HttpServer m_http;
        WSServer m_ws;
    };
}

#endif // APPLICATIONS_API_H
