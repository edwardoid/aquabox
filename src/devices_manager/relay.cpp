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
#include "hardware.h"
#include <common/log.h>

using namespace aquabox;

Relay::Relay() { }

nlohmann::json Relay::value(std::string property) const
{
    if (property == ON_OFF_PROPERTY) {
        return turnedOn() ? 1 : 0;
    }

    return Device::value(property);
};

bool Relay::setValue(std::string property, const nlohmann::json& value)
{
    if (property == ON_OFF_PROPERTY) {
        if (value.is_number()) {
            if (value == 0) {
                turnOff();
            } else if (value == 1) {
                turnOn();
            }
            return true;
        }
    }
    return false;
}

bool Relay::turnedOn() const
{
    auto hw = Hardware::resolve(m_address);
    if (hw == nullptr) {
        LOG_ERROR << "Can't find device " << m_address;
        return false;
    }

    auto impl = hw->implementation<OnOff>();

    if (impl == nullptr) {
        LOG_ERROR << "Can't find device " << m_address;
        return false;
    }

    return impl->isOn();
}

void Relay::turnOn()
{
    if (turnedOn()) {
        return;
    }

    auto hw = Hardware::resolve(m_address);
    if (hw == nullptr) {
        LOG_ERROR << "Can't find device " << m_address;
        return;
    }

    auto impl = hw->implementation<OnOff>();

    if (impl == nullptr) {
        LOG_ERROR << m_address << " does not implement OnOff interface.";
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

    auto hw = Hardware::resolve(m_address);
    if (hw == nullptr) {
        LOG_ERROR << "Can't find device " << m_address;
        return;
    }

    auto impl = hw->implementation<OnOff>();

    if (impl == nullptr) {
        LOG_ERROR << m_address << " does not implement OnOff interface.";
        return;
    }

    impl->setOff();

    LOG_DEBUG << "Turned off";
}

void Relay::addPendingAction(uint64_t priority, ValueChange action)
{
    m_pendingActions.insert({ priority, action });
}

bool Relay::performPendingAction(ValueChange& action)
{
    if (m_pendingActions.empty()) {
        return false;
    }

    auto lb = m_pendingActions.lower_bound(0);

    ValueChange actionToPerform;
    actionToPerform.property = ON_OFF_PROPERTY;
    actionToPerform.value = 1;
    if (lb == m_pendingActions.begin()) {
        actionToPerform = m_pendingActions.rbegin()->second;
    } else {
        if (lb->first != 0) {
            --lb;
        }
        actionToPerform = lb->second;
    }

    m_pendingActions.clear();

    if ((actionToPerform.value == 1) == turnedOn()) {
        return false;
    }

    if (actionToPerform.value == 1) {
        turnOn();
    } else {
        turnOff();
    }

    action = actionToPerform;

    return true;
}