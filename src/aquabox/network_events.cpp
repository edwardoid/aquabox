
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

using namespace aquabox;

NetworkChangedEvent::NetworkChangedEvent()
{
    CurrentNetworkState net = (CurrentNetworkState) NetworkManager::i().currentNetwork;
    setData({
        { "network", net.ssid },
        { "failed", false},
        { "auth", true }
    });
    setPropertiesUpdate({
        { "ssid", net.ssid },
        { "uuid", net.uuid },
        { "ip", net.ip },
        { "strength", net.strength }
    });
}

NetworkConnectionFailedEvent::NetworkConnectionFailedEvent()
{
    CurrentNetworkState net = (CurrentNetworkState) NetworkManager::i().currentNetwork;
    setData({
        { "network", net.ssid },
        { "failed", true},
        { "auth", true }
    });
    setPropertiesUpdate({
        { "ssid", net.ssid },
        { "uuid", net.uuid }
    });
}

NetworkAuthentifiationFailedEvent::NetworkAuthentifiationFailedEvent()
{
    CurrentNetworkState net = (CurrentNetworkState) NetworkManager::i().currentNetwork;
    setData({
        { "network", net.ssid },
        { "failed", true},
        { "auth", false }
    });
    setPropertiesUpdate({
        { "ssid", net.ssid },
        { "uuid", net.uuid }
    });
}
