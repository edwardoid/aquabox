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

Relay::Relay()
{
}

bool Relay::turnedOn() const
{
    auto hw = Hardware::resolve(getAddress().Value);
    if (hw == nullptr) {
        LOG_ERROR << "Can't find device " << getAddress().Value;
        return false;
    }

    auto impl = hw->implementation<OnOffDevice>();

    if (impl == nullptr) {
        LOG_ERROR << "Can't find device " << getAddress().Value;
        return false;
    }

    return impl->isOn();
}

void Relay::turnOn()
{
    if (turnedOn()) {
        return;
    }

    auto hw = Hardware::resolve(getAddress().Value);
    if (hw == nullptr) {
        LOG_ERROR << "Can't find device " << getAddress().Value;
        return;
    }

    auto impl = hw->implementation<OnOffDevice>();

    if (impl == nullptr) {
        LOG_ERROR << getAddress().Value << " does not implement OnOff interface.";
        return;
    }

    impl->setOn();

    LOG_DEBUG << "Turned on";
}

void Relay::turnOff()
{
    if (!turnedOn()) {
        return;
    }

    auto hw = Hardware::resolve(getAddress().Value);
    if (hw == nullptr) {
        LOG_ERROR << "Can't find device " << getAddress().Value;
        return;
    }

    auto impl = hw->implementation<OnOffDevice>();

    if (impl == nullptr) {
        LOG_ERROR << getAddress().Value << " does not implement OnOff interface.";
        return;
    }

    impl->setOff();

    LOG_DEBUG << "Turned off";
}

void Relay::addPendingAction(uint64_t priority, ActionType action)
{
    m_pendingActions.insert({priority, action});
}

bool Relay::performPendingAction()
{
    if (m_pendingActions.empty()) {
        return false;
    }

    auto lb = m_pendingActions.lower_bound(0);

    ActionType actionToPerform = ActionType::TurnOn;
    if (lb == m_pendingActions.begin()) {
        actionToPerform = m_pendingActions.rbegin()->second;
    } else {
        if (lb->first != 0) {
            --lb;
        }
        actionToPerform = lb->second;
    }

    m_pendingActions.clear();

    if ((actionToPerform == ActionType::TurnOn) == turnedOn())
    {
        return false;
    }

    switch(actionToPerform)
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
    return true;
}