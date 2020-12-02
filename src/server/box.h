#ifndef BOX_H
#define BOX_H

#include "box_connection.h"
#include "event.h"
#include <s2s.h>

namespace aquabox
{
    class ApplicationConnection;

    class Box: public BoxConnection
    {
    public:
        Box(ResultsMap<Command>& results);
        SignalSlot::Signal<const nlohmann::json&> UpdateReceived;
        void addConnection(std::shared_ptr<ApplicationConnection> connection);
        void removeConnection(std::shared_ptr<ApplicationConnection> connection);
        std::vector<std::shared_ptr<ApplicationConnection>> connections() const;
    private:
        std::vector<std::shared_ptr<ApplicationConnection>> m_applications;
    };
}

#endif // BOX_H