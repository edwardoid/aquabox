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

#include <string>
#include <vector>
#include <mutex>
#include <s2s_property.h>
#include <nmdbus/NetworkManager.h>
#include <nmdbus/AccessPoint.h>

namespace aquabox
{
    struct CurrentNetworkState
    {
        std::string uuid;
        std::string ssid;
        std::string ip;
        int strength = 0;
        bool hasInternet = false;
        bool operator == (const CurrentNetworkState& o) const {
            return  uuid == o.uuid &&
                    ssid == o.ssid &&
                    ip == o.ip &&
                    strength == o.strength &&
                    hasInternet == hasInternet;
        }
    };

    using NetworkingState = nm::NetworkingState;

    struct WifiNetwork
    {
        std::string ssid;
        std::string password;
        std::string ip;
        nm::Security auth;
        bool encrypted = 0;
        int signal = 0;

        const bool operator == (const WifiNetwork& wifi)
        {
            return ssid == wifi.ssid && auth == wifi.auth && encrypted == wifi.encrypted;
        }

        const bool operator < (const WifiNetwork& wifi)
        {
            if (signal < wifi.signal) { return true; }
            return ssid < wifi.ssid; 
        }
    };

    struct Connection: public WifiNetwork
    {
        std::string uuid;
    };

    class NetworkManager
    {
        NetworkManager();
        ~NetworkManager();
    public:

        static NetworkManager& i();
        bool connectedToInternet();
        void setInterface(std::string interface);
        bool requestScan(bool force = true);
        SignalSlot::Property<CurrentNetworkState> currentNetwork;
        SignalSlot::Property<NetworkingState> currentState;
        std::vector<WifiNetwork> networks();
        bool connect(const std::string& ssid, const std::string& password);
        bool activateConnection(std::string uuid);
        SignalSlot::Signal<size_t> NetworksChanged;
        SignalSlot::Signal<> AuthentificationFailed;
        SignalSlot::Signal<> ConnectingFailed;
        void updateCurrentState();
    private:
        void reconnect();
    private:
        std::string m_interface;
        std::thread m_reconnectThread;
        std::mutex m_connectingInProgress;
        nm::NetworkManager m_nm;
    };
}
