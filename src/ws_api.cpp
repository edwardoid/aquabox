#include "ws_api.h"
#include "event.h"

using namespace aquabox::ws;

void API::onOpen(const std::string& route, Connection connection)
{
    std::lock_guard<std::mutex> guard(m_lock);

    auto connections = m_connections.find(route);
    if (connections == m_connections.end()) {
        m_connections.insert({ route, std::vector<Connection>({ connection })});
    } else  {
        connections->second.push_back(connection);
    }
}
void API::onClose(const std::string& route, Connection connection, Status status, const std::string& reason) 
{
    std::lock_guard<std::mutex> guard(m_lock);

    auto connections = m_connections.find(route);
    if (connections == m_connections.end()) {
        return;
    }

    for(auto i = connections->second.begin(); i != connections->second.end(); ++i)
    {
        if(i->get() == connection.get())
        {
            connections->second.erase(i);
            break;
        }
    }
}

void API::postEvent(Event event)
{
    std::lock_guard<std::mutex> guard(m_lock);
    auto data = event->serialize().dump(4);

    auto connections = m_connections.find(UpdatesUrl);
    if (connections == m_connections.end()) {
        return;
    }

    for(auto& connection : connections->second)
    {
        connection->send(data);
    }
}

DEFINE_WS_API_IMPL(Updates)
{
    connection->send("Updates");
}