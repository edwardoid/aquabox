#ifndef APPLICATIONS_API_H
#define APPLICATIONS_API_H

#include "rest_api.h"
#include "iot_definitions.h"

namespace aquabox
{
    class ApplicationsApi
    {
    public:
        static void start();
    private:
        void startWebServer();
        void startStreamServer();
        void removeConnection(std::string boxId, std::shared_ptr<WSServer::Connection> connection);
    private:
        static ApplicationsApi m_instance;
    private:
        Route m_base;
        std::shared_ptr<HttpServer> m_http;
        std::shared_ptr<WSServer> m_ws;
        v1::REST RESTApi;
    };
}

#endif // APPLICATIONS_API_H
