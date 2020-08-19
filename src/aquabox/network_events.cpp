
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

#include "network_events.h"
#include "network_manager.h"
#include "network_manager_device_state.h"
#include "enumerations.h"

using namespace aquabox;

NetworkDeviceStateEvent::NetworkDeviceStateEvent()
{
    Connection net = (Connection) NetworkManager::i().currentConnection;
    setData({
        { "state", Serializable<nm::NMDeviceState>::convert((nm::NMDeviceState )NetworkManager::i().deviceState) }
    });
    setPropertiesUpdate({
        { "ssid", net.ssid },
        { "uuid", net.uuid }
    });
}

NetworkChangedEvent::NetworkChangedEvent()
{
    Connection net = (Connection) NetworkManager::i().currentConnection;
    setData({
        { "network", net.ssid },
        { "reason", Serializable<NetworkChangeReason>::convert(NetworkManager::i().changeReason()) }
    });
    setPropertiesUpdate({
        { "ssid", net.ssid },
        { "uuid", net.uuid },
        { "ip", net.ip },
        { "strength", net.signal }
    });
}

NetworkConnectionFailedEvent::NetworkConnectionFailedEvent()
{
    Connection net = (Connection) NetworkManager::i().currentConnection;
    setData({
        { "network", net.ssid },
        { "reason", "connectoin_failed"}
    });
    setPropertiesUpdate({
        { "ssid", net.ssid },
        { "uuid", net.uuid }
    });
}

NetworkAuthentifiationFailedEvent::NetworkAuthentifiationFailedEvent()
{
    Connection net = (Connection) NetworkManager::i().currentConnection;
    setData({
        { "network", net.ssid },
        { "reason", "authentification_failed"}
    });
    setPropertiesUpdate({
        { "ssid", net.ssid },
        { "uuid", net.uuid }
    });
}
