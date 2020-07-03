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

#include "network_manager.h"
#include "settings.h"
#include "log.h"
#include <nmdbus/Device.h>
#include <nmdbus/WirelessDevice.h>
#include <nmdbus/ActiveConnection.h>
#include <nmdbus/Ip4Config.h>
#include <nmdbus/Connection.h>
#include <mutex>
#include <thread>

using namespace aquabox;

NetworkManager::NetworkManager()
    : m_reconnectThread(std::bind(&NetworkManager::reconnect, this))
{
    m_nm.ActiveConnectionsChanged.connect([this](const std::vector<std::shared_ptr<nm::ActiveConnection>>&) {
        auto dev = m_nm.getDevice(m_interface);
        if (dev == nullptr) {
            LOG_ERROR << "Can't find device " << m_interface;
            return;
        }
        updateCurrentState();
    });

    m_nm.StateChanged.connect(currentState);

    m_reconnectThread.detach();
}

NetworkManager::~NetworkManager()
{}

NetworkManager& NetworkManager::i()
{
    static NetworkManager instance;
    return instance;
}

void NetworkManager::reconnect()
{
    while(1) {
        std::this_thread::sleep_for (std::chrono::minutes(1));
        if(Settings::has(Settings::Keys::NETWORK) && currentState == NetworkingState::Disconnected)
        {
            auto uuid = Settings::string(Settings::Keys::NETWORK, currentNetwork.value.uuid);
            if (uuid.empty()) {
                continue;
            }

            if (m_connectingInProgress.try_lock()) {
                try {
                    activateConnection(uuid);
                }
                catch(std::exception& ex) {
                    LOG_ERROR << "Reconnect failed: " << ex.what();
                }
                catch(...) {
                    LOG_ERROR << "Reconnect failed...";
                }
                m_connectingInProgress.unlock();
            }
        }
        updateCurrentState();
    }
}

void NetworkManager::updateCurrentState()
{
    CurrentNetworkState newValue;
    do {
        try {
            auto dev = m_nm.getDevice(m_interface);
            if (dev == nullptr) {
                LOG_ERROR << "Can't find device " << m_interface;
                break;
            }

            auto ac = dev->activeConnection();
            if (ac == nullptr) {
                LOG_DEBUG << "No active connection at this moment!";
                break;
            }

            auto c = ac->connection();
            if (c == nullptr) {
                LOG_ERROR << "Can't find conenction instance!";
                break;
            }

            nm::ConnectionSettings connectionSettings = c->settings();

            auto uuidPos = connectionSettings.connection.find(CONNECTION_DATA_WIFI_UUID);

            if (uuidPos != connectionSettings.connection.end()) {
                auto& uuid = uuidPos->second;
                if (uuid.get<std::string>() != nullptr) {
                    newValue.uuid = *(uuid.get<std::string>());
                }
            }

            auto wifi = dev->asWireless();
            if (wifi == nullptr) {
                LOG_ERROR << "Device " << m_interface << " is not a WiFi device";
                break;
            }

            auto ap = wifi->activeAccessPoint();
            if (ap != nullptr) {
                newValue.ssid = ap->ssid();
                newValue.strength = ap->strength();
            } else {
                LOG_DEBUG << "No Access point info available";
            }

            newValue.hasInternet = m_nm.state() == nm::NetworkingState::ConnectedGlobal;

            if (!newValue.uuid.empty()) {
                Settings::set(Settings::Keys::NETWORK, newValue.uuid); // Update
            }

            auto ipv4 = dev->ipv4Configuration();
            if (ipv4 == nullptr) {
                LOG_ERROR << "Can't get IPv4 configuration";
                break;
            }

            newValue.ip = ipv4->ip();
        }
        catch (std::exception& ex) {
            LOG_ERROR << ex.what();
            break;
        }
    
    } while(false);

    currentNetwork.set(newValue);
}

void NetworkManager::setInterface(std::string interface)
{
    m_interface = interface;
    if (m_interface.empty()) {
        return;
    }
    auto dev = m_nm.getDevice(m_interface);
    if (dev != nullptr) {
        dev->StateChanged.connect([this](nm::NMDeviceState newState, nm::NMDeviceState oldState) {
            LOG_ERROR << "State changed from " << oldState << " to " << newState;
            if (newState == nm::NM_DEVICE_STATE_NEED_AUTH) {
                AuthentificationFailed.emit();
            } else if (newState == nm::NM_DEVICE_STATE_FAILED) {
                ConnectingFailed.emit();
            }
            updateCurrentState();
        });
    }

}

bool NetworkManager::requestScan(bool force)
{
    auto dev = m_nm.getDevice(m_interface);
    if (dev == nullptr) {
        return false;
    }

    auto wifi = dev->asWireless();
    if (wifi == nullptr) {
        return false;
    }

    return wifi->scan();
}

bool NetworkManager::connectedToInternet()
{
    return m_nm.getConnectivity() == nm::ConnectivityState::Full;
}

std::vector<WifiNetwork> NetworkManager::networks()
{
    std::vector<WifiNetwork> networks;

    auto dev = m_nm.getDevice(m_interface);
    if (dev == nullptr) {
        return networks;
    }

    auto wifi = dev->asWireless();
    if (wifi == nullptr) {
        return networks;
    }

    for(auto ap : wifi->accessPoints()) {
        WifiNetwork net;
        net.ssid = ap->ssid();
        net.auth = ap->security();
        net.encrypted = ap->encrypted();
        net.signal = ap->strength();
        networks.push_back(net);
    }

    return networks;
}

bool NetworkManager::connect(const std::string& ssid, const std::string& password)
{
    std::lock_guard<std::mutex> lock(m_connectingInProgress);
    return m_nm.connect(m_interface, ssid, password);

}

bool NetworkManager::activateConnection(std::string uuid)
{
    auto conn = m_nm.getConnectionByUuid(uuid);
    if (conn == nullptr) {
        return false;
    }

    auto dev = m_nm.getDevice(m_interface);
    if (dev == nullptr) {
        return false;
    }

    return m_nm.activate(conn, dev);
}
