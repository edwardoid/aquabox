#include "applications_api.h"

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

}