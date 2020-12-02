/*
 * AquaBox
 * Copyright (C) 2020  Eduard Sargsyan
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

#pragma once

#include "event.h"
#include "network_change_reason.h"
#include "network_manager.h"
#include "network_manager_device_state.h"

namespace aquabox
{
    class NetworkDeviceStateEvent: public BoxEvent { public: NetworkDeviceStateEvent(const nm::NMDeviceState& state); };

    class NetworkChangedEvent: public BoxEvent { public: NetworkChangedEvent(const Connection& connection); };

    class NetworkConnectionFailedEvent: public BoxEvent { public: NetworkConnectionFailedEvent(); };

    class NetworkAuthentifiationFailedEvent: public BoxEvent { public: NetworkAuthentifiationFailedEvent(); };
}