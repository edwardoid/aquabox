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

#pragma once

#include <stdint.h>
#include <string>
#include <vector>

namespace aquabox {

enum class NetworkState
{
    Disconnecting = 0,
    Connecting = 1,
    Authentificating = 2,
    OptainingIP = 3,
    Connected = 4,
    Disconnected = 5,
    Failed = 6
};


struct NetworkInfo
{
    std::string uuid;
    std::string ssid;
    std::string ip;
    std::string auth;
    int strength;
    bool secure;
};

class INetworkManager {
public:
    virtual NetworkState currentState() = 0;
    virtual NetworkInfo currentNetwork() = 0;
    virtual bool startScan(bool force = true) = 0;
    virtual bool hasInternetConnection() = 0;
    virtual std::vector<NetworkInfo> availableNetworks() = 0;
    virtual std::string connect(const std::string ssid, const std::string password) = 0;
};
}