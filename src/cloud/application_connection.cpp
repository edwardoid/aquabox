#include "application_connection.h"
#include "box.h"
#include "event.h"

using namespace aquabox;

ApplicationConnection::ApplicationConnection(Box& box, std::shared_ptr<Web::WSConnection> connection)
    : m_box(box), m_connection(connection)
{
    m_updateConnection = m_box.UpdateReceived.connect([this](const nlohmann::json& event){
        m_connection->send(event.dump());
    });
}

ApplicationConnection::~ApplicationConnection() {
    m_box.UpdateReceived.disconnect(m_updateConnection);
}

bool ApplicationConnection::operator == (const std::shared_ptr<Web::WSConnection> connection) const
{
    return m_connection == connection;
}
