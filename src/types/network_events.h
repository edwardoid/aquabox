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

#include <nmdbus/Device.h>
#include <nmdbus/AccessPoint.h>
#include <types/event.h>

struct WifiNetwork {
    std::string ssid;
    std::string password;
    std::string ip;
    nm::Security auth = nm::Security::Unknown;
    bool encrypted = 0;
    int signal = 0;

    const bool operator==(const WifiNetwork& wifi)
    {
        return ssid == wifi.ssid && auth == wifi.auth && encrypted == wifi.encrypted && signal == wifi.signal;
    }

    const bool operator<(const WifiNetwork& wifi)
    {
        if (signal < wifi.signal) {
            return true;
        }
        return ssid < wifi.ssid;
    }
};

struct Connection : public WifiNetwork {
    std::string uuid;
};

namespace aquabox {
    using NetworkDeviceState = nm::NMDeviceState;
class NetworkEvent : public Event {
public:
    NetworkEvent();
};

class NetworkDeviceStateEvent : public NetworkEvent {
public:
    NetworkDeviceStateEvent(const nm::NMDeviceState& state);
};

class NetworkChangedEvent : public NetworkEvent {
public:
    NetworkChangedEvent(const Connection& connection);
};

class NetworkConnectionFailedEvent : public NetworkEvent {
public:
    NetworkConnectionFailedEvent();
};

class NetworkAuthentifiationFailedEvent : public NetworkEvent {
public:
    NetworkAuthentifiationFailedEvent();
};
}