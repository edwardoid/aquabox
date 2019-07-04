#include "wsapi.h"
#include "event.h"

using namespace aquabox::ws;

void API::onOpen(Connection connection)
{
    std::lock_guard<std::mutex> guard(m_lock);
    m_connections.push_back(connection);
}
void API::onClose(Connection connection, Status status, const std::string& reason) 
{
    std::lock_guard<std::mutex> guard(m_lock);
    for(auto i = m_connections.begin(); i != m_connections.end(); ++i)
    {
        if(i->get() == connection.get())
        {
            m_connections.erase(i);
            break;
        }
    }
}

void API::postEvent(Event event)
{
    std::lock_guard<std::mutex> guard(m_lock);
    auto data = event->serialize().dump(4);
    for(auto& connection : m_connections)
    {
        connection->send(data);
    }
}

DEFINE_WS_API_IMPL(Ping)
{
    connection->send("msg");
}