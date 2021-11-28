
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

#include "network_manager_events.h"
#include "network_manager.h"
#include <common/enumerations.h>
#include <common/singleton.h>

using namespace aquabox;

NetworkDeviceStateEvent::NetworkDeviceStateEvent(
    const nm::NMDeviceState& state)
{
    auto nm = Singleton<NetworkManager>::i();

    if (nm == nullptr) {
        return;
    }
    Connection net = (Connection)nm->currentConnection;
    setData({ { "state", Serializable<nm::NMDeviceState>::convert(state) } });
    setPropertiesUpdate({ { "ssid", net.ssid }, { "uuid", net.uuid } });
}

NetworkChangedEvent::NetworkChangedEvent(const Connection& net)
{
    auto nm = Singleton<NetworkManager>::i();

    if (nm == nullptr) {
        return;
    }
    setData({ { "network", net.ssid },
        { "reason",
            Serializable<NetworkChangeReason>::convert(nm->changeReason()) } });
    setPropertiesUpdate({ { "ssid", net.ssid },
        { "uuid", net.uuid },
        { "ip", net.ip },
        { "strength", net.signal } });
}

NetworkConnectionFailedEvent::NetworkConnectionFailedEvent()
{
    auto nm = Singleton<NetworkManager>::i();

    if (nm == nullptr) {
        return;
    }

    Connection net = (Connection)nm->currentConnection;
    setData({ { "network", net.ssid }, { "reason", "connectoin_failed" } });
    setPropertiesUpdate({ { "ssid", net.ssid }, { "uuid", net.uuid } });
}

NetworkAuthentifiationFailedEvent::NetworkAuthentifiationFailedEvent()
{
    auto nm = Singleton<NetworkManager>::i();

    if (nm == nullptr) {
        return;
    }

    Connection net = (Connection)nm->currentConnection;
    setData({ { "network", net.ssid }, { "reason", "authentification_failed" } });
    setPropertiesUpdate({ { "ssid", net.ssid }, { "uuid", net.uuid } });
}
