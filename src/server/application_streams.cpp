#include "application_streams.h"

using namespace aquabox;

bool ApplicationStreams::hasConnectionToBox(const std::string& box)
{
    return m_connections.count(box) > 0;
}

std::shared_ptr<ApplicationStreams::Connection> ApplicationStreams::getConnectionToBox(const std::string& box)
{
    return m_connections[box];
}

void ApplicationStreams::addConnectionTo(const std::string& box, std::shared_ptr<Connection> connection)
{
    m_connections.insert({box, connection});
}

bool ApplicationStreams::removeConnectionTo(const std::string& box)
{
    auto pos = m_connections.find(box);
    if (pos != m_connections.end()) {
        m_connections.erase(pos);
        return true;
    }
    return false;
}
