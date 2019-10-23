/*
 * AquaBox
 * Copyright (C) 2019  Eduard Sargsyan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "relay.h"

#include "gpio.h"

using namespace aquabox;

bool Relay::turnedOn() const
{
    auto data = read(1);
    return data.size() > 0 && data[0] == 0;
}

void Relay::turnOn()
{
    if (turnedOn()) {
        return;
    }

    auto hw = Hardware::resolve(getAddress());
    if (hw == nullptr) {
        LOG_ERROR << "Can't find device " << getAddress();
        return;
    }
    
    static Bytes ON({ 0 });
    if (ON.size() != hw->send(ON, ON.size())) {
        LOG_ERROR << "Can't write to HW " << getAddress();
    }

    LOG_DEBUG << "Turned on";
}

void Relay::turnOff()
{
    auto hw = Hardware::resolve(getAddress());
    if (hw == nullptr) {
        LOG_ERROR << "Can't find device " << (std::string)(getAddress());
        return;
    }

    if (!turnedOn()) {
        return;
    }

    static Bytes OFF({ 1 });
    if (OFF.size() != hw->send(OFF, OFF.size())) {
        LOG_ERROR << "Can't write to HW " << (std::string)(getAddress());
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
    if (m_pendingActionPriority != INVALID_PRIORITY && getRulesEnabled().Value) {
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