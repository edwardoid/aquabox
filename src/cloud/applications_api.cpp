#include "applications_api.h"
#include "route.h"

using namespace aquabox;

ApplicationsApi ApplicationsApi::m_instance;

void ApplicationsApi::start()
{
    static std::thread routine([]() {
        m_instance.startServer();
    });
}

void ApplicationsApi::startServer()
{
    m_http = std::make_shared<v1::API::HttpServer>();
    m_http->config.port = 1311;
    LOG_INFO << "Starting web server on " << m_http->config.address << ":" << m_http->config.port;
    const Route base = Route().word("api").word("v1").word("app").string(":appId");
    RESTApi.init(m_http, base);
    
    m_http->start();
}