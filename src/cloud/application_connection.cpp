#include "application_connection.h"
#include "box.h"
#include "event.h"

using namespace aquabox;

ApplicationConnection::ApplicationConnection(Box& box, std::shared_ptr<ApplicationConnection::WSServer::Connection> connection)
    : m_box(box), m_connection(connection)
{
    m_updateConnection = m_box.UpdateReceived.connect([this](std::shared_ptr<Event> event){
        auto data = event->serialize().dump(4);
        m_connection->send(data);
    });
}

ApplicationConnection::~ApplicationConnection() {
    m_box.UpdateReceived.disconnect(m_updateConnection);
}

