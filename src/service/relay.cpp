#include "relay.h"

#include "gpio.h"

using namespace aquabox;

bool Relay::turnedOn() const
{
    auto hw = Hardware::resolve(getAddress().Value);
    if (hw == nullptr) {
        LOG_ERROR << "Can't find device " << getAddress().Value;
        return false;
    }

    static Bytes val({ 0 });

    return hw->read(val, 0) == 1 && val[0] == 1;
}

void Relay::turnOn()
{
    auto hw = Hardware::resolve(getAddress().Value);
    if (hw == nullptr) {
        LOG_ERROR << "Can't find device " << getAddress().Value;
        return;
    }
    
    if (turnedOn()) {
        return;
    }

    static Bytes ON({ 0 });
    if (ON.size() != hw->send(ON, ON.size())) {
        LOG_ERROR << "Can't write to HW " << getAddress().Value;
    }

    LOG_DEBUG << "Turned on";
}

void Relay::turnOff()
{
    auto hw = Hardware::resolve(getAddress().Value);
    if (hw == nullptr) {
        LOG_ERROR << "Can't find device " << getAddress().Value;
        return;
    }

    if (!turnedOn()) {
        return;
    }

    static Bytes OFF({ 1 });
    if (OFF.size() != hw->send(OFF, OFF.size())) {
        LOG_ERROR << "Can't write to HW " << getAddress().Value;
    }

    LOG_DEBUG << "Turned off";
}

void Relay::addPendingAction(uint64_t priority, ActionType action)
{
    if (m_pendingActionPriority == INVALID_PRIORITY || m_pendingActionPriority >= priority) {
        m_pendingAction = action;
        m_pendingActionPriority = priority;
    }
}

bool Relay::performPendingAction()
{
    if (m_pendingActionPriority != INVALID_PRIORITY) {
        switch(m_pendingAction)
        {
            case ActionType::TurnOn: {
                turnOn();
                break;
            }
            case ActionType::TurnOff: {
                turnOff();
                break;
            }
        }
        m_pendingActionPriority = INVALID_PRIORITY;
        return true;
    }

    return false;
}