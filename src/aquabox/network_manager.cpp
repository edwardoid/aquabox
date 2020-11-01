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
    , m_changeReason(NetworkChangeReason::NothingSpecial)
    , currentState(NetworkingState::Unknown)
    , deviceState(NetworkDeviceState::NM_DEVICE_STATE_UNKNOWN)
{
    m_nm.ActiveConnectionsChanged.connect(std::bind(&NetworkManager::onActiveConnectionChanged, this, std::placeholders::_1));

    m_nm.StateChanged.connect(currentState);

    m_reconnectThread.detach();
}

void NetworkManager::reconnect()
{
    while(1) {
        std::this_thread::sleep_for (std::chrono::minutes(1));
        if(Settings::has(Settings::Keys::NETWORK) && currentState == NetworkingState::Disconnected)
        {
            m_changeReason = NetworkChangeReason::Reconnect;
            auto uuid = Settings::string(Settings::Keys::NETWORK, currentConnection.value.uuid);
            if (uuid.empty()) {
                continue;
            }

            if (m_connectingInProgress.try_lock()) {
                try {
                    LOG_INFO << "Reconnecting...";
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
        if (!m_connectingInProgress.try_lock())
            return;
        updateCurrentState();
        m_connectingInProgress.unlock();
    }
}

void NetworkManager::updateCurrentState(bool notify)
{
    Connection newValue;
    do {
        try {
            if (m_dev == nullptr) {
                LOG_ERROR << "Can't find device " << m_interface;
                break;
            }

            auto ac = m_dev->activeConnection();
            if (ac == nullptr) {
                LOG_DEBUG << "No active connection at this moment!";
                break;
            }

            auto c = ac->connection();
            if (c == nullptr) {
                LOG_ERROR << "Can't find conection instance!";
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

            auto wifi = m_dev->asWireless();
            if (wifi == nullptr) {
                LOG_ERROR << "Device " << m_interface << " is not a WiFi device";
                break;
            }

            auto ap = wifi->activeAccessPoint();
            if (ap != nullptr) {
                newValue.ssid = ap->ssid();
                newValue.signal = ap->strength();
                newValue.encrypted = ap->encrypted();
            } else {
                LOG_DEBUG << "No Access point info available";
            }

            if (!newValue.uuid.empty()) {
                Settings::set(Settings::Keys::NETWORK, newValue.uuid); // Update
            }

            auto ipv4 = m_dev->ipv4Configuration();
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

    currentConnection.blockSignals(!notify);
    currentConnection.set(newValue);
    currentConnection.blockSignals(false);
}

NetworkChangeReason NetworkManager::changeReason() const
{
    return m_changeReason;
}

void NetworkManager::setInterface(std::string interface)
{
    m_interface = interface;
    if (m_interface.empty()) {
        return;
    }
    m_dev = m_nm.getDevice(m_interface);
    if (m_dev != nullptr) {
        m_dev->StateChanged.connect(std::bind(&NetworkManager::onDeviceStateChanged, this, std::placeholders::_1, std::placeholders::_2));
    }

}

void NetworkManager::onDeviceStateChanged(nm::NMDeviceState newState, nm::NMDeviceState oldState)
{
    updateCurrentState(newState == nm::NMDeviceState::NM_DEVICE_STATE_ACTIVATED);
    deviceState.set(newState);
    LOG_ERROR << "State changed from " << oldState << " to " << newState;
    if (newState == nm::NM_DEVICE_STATE_FAILED && oldState == nm::NM_DEVICE_STATE_NEED_AUTH) {
        AuthentificationFailed.emit();
    } else if (newState == nm::NM_DEVICE_STATE_FAILED) {
        ConnectingFailed.emit();
    } else if (newState == nm::NM_DEVICE_STATE_ACTIVATED) {
        
    }
}

void NetworkManager::onActiveConnectionChanged(const std::vector<std::shared_ptr<nm::ActiveConnection>>&)
{
    if (m_dev == nullptr) {
        LOG_ERROR << "Can't find device " << m_interface;
        return;
    }
    if (!m_connectingInProgress.try_lock()) return;
    updateCurrentState();
    m_connectingInProgress.unlock();
}

bool NetworkManager::requestScan(bool force)
{
    if (m_dev == nullptr) {
        return false;
    }

    auto wifi = m_dev->asWireless();
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

    if (m_dev == nullptr) {
        return networks;
    }

    auto wifi = m_dev->asWireless();
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

std::string NetworkManager::connect(const std::string& ssid, const std::string& password)
{
    std::lock_guard<std::mutex> lock(m_connectingInProgress);
    return m_nm.connect(m_interface, ssid, password);

}

bool NetworkManager::activateConnection(std::string uuid)
{
    LOG_DEBUG << "Activating connection " << uuid;
    auto conn = m_nm.getConnectionByUuid(uuid);
    if (conn == nullptr) {
        return false;
    }

    if (m_dev == nullptr) {
        return false;
    }

    return m_nm.activate(conn, m_dev);
}
