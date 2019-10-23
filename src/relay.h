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