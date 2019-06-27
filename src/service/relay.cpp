#include "relay.h"

#include "gpio.h"

using namespace aquabox;

bool Relay::turnedOn() const
{
    auto gpio = GPIO::get(getPin().Value);
    return !gpio->isHigh();
}

void Relay::turnOn()
{
    auto gpio = GPIO::get(getPin().Value);
    if (gpio == nullptr) {
        LOG_ERROR << "Can't find attached GPIO " << (int)getPin().Value;
    }

    if (turnedOn()) {
        return;
    }

    static Bytes ON({ 0 });
    if (ON.size() != gpio->send(ON, ON.size())) {
        LOG_ERROR << "Can't write to GPIO " << (int)getPin().Value;
    }

    LOG_DEBUG << "Turned on";
}

void Relay::turnOff()
{
    auto gpio = GPIO::get(getPin().Value);
    if (gpio == nullptr) {
        LOG_ERROR << "Can't find attached GPIO " << (int)getPin().Value;
    }

    if (!turnedOn()) {
        return;
    }

    static Bytes OFF({ 1 });
    if (OFF.size() != gpio->send(OFF, OFF.size())) {
        LOG_ERROR << "Can't write to GPIO " << (int)getPin().Value;
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