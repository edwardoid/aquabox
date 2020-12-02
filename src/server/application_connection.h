#ifndef APPLICATION_CONNECTION_H
#define APPLICATION_CONNECTION_H

#include "iot_definitions.h"

namespace aquabox
{
    class Box;
    class ApplicationConnection
    {
    public:
        ApplicationConnection(Box& box, std::shared_ptr<Web::WSConnection> connection);
        ~ApplicationConnection();
        bool operator == (const std::shared_ptr<Web::WSConnection> connection) const;
    private:
        int m_updateConnection = -1;
        Box& m_box;
        std::shared_ptr<Web::WSConnection> m_connection;
    };
}

#endif // APPLICATION_CONNECTION_H