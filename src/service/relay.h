#ifndef RELAY_H
#define RELAY_H

#include "device.h"

namespace aquabox
{
    class Relay: public Device
    {
        static const uint64_t INVALID_PRIORITY = static_cast<uint64_t>(-1);
    public:
        Relay() = default;
        Relay(const Relay& src) = default;
        Relay(Relay&& src) = default;

        bool turnedOn() const;

        void turnOn();
        void turnOff();

        void addPendingAction(uint64_t priority, ActionType action);
        bool performPendingAction();
    private:
        uint64_t m_pendingActionPriority = INVALID_PRIORITY;
        ActionType m_pendingAction;
    };
}

#endif // RELAY_H