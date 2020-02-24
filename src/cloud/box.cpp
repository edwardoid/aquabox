#include "box.h"
#include "log.h"
#include "application_connection.h"

using namespace aquabox;

Box::Box(ResultsMap<Command>& results)
    : BoxConnection(results)
{}

std::vector<std::shared_ptr<ApplicationConnection>> Box::connections() const
{
    return m_applications;
}

void Box::addConnection(std::shared_ptr<ApplicationConnection> connection)
{
    LOG_INFO << "Addind application connection";
    m_applications.push_back(connection);
}

void Box::removeConnection(std::shared_ptr<ApplicationConnection> connection)
{
    LOG_INFO << "Removing application connection";
    auto p = std::find(m_applications.begin(), m_applications.end(), connection);
    if (p != m_applications.end())
        m_applications.erase(p);
}
