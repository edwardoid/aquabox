#ifndef APPLICATIONS_API_H
#define APPLICATIONS_API_H

#include <server_http.hpp>
#include <server_ws.hpp>
#include "rest_api.h"

namespace aquabox
{
    class ApplicationsApi
    {
        using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
        using WSServer = SimpleWeb::SocketServer<SimpleWeb::WS>;
    public:
        static void start();
    private:
        void startServer();
    private:
        static ApplicationsApi m_instance;
    private:
        std::shared_ptr<HttpServer> m_http;
        v1::REST RESTApi;
        WSServer m_ws;
    };
}

#endif // APPLICATIONS_API_H
