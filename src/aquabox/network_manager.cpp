#include "network_manager.h"

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
{
    m_nm.ActiveConnectionsChanged.connect([this](const std::vector<std::shared_ptr<nm::ActiveConnection>>&) {
        updateCurrentState();
    });
}

NetworkManager::~NetworkManager()
{}

NetworkManager& NetworkManager::i()
{
    static NetworkManager instance;
    return instance;
}

void NetworkManager::updateCurrentState()
{
    CurrentNetworkState newValue = currentNetwork.value;
    do {
        auto dev = m_nm.getDevice(m_interface);
        if (dev == nullptr) {
            LOG_ERROR << "Can't find device " << m_interface;
            break;
        }

        auto wifi = dev->asWireless();
        if (wifi == nullptr) {
            LOG_ERROR << "Device " << m_interface << " is not a WiFi device";
            break;
        }

        auto ipv4 = dev->ipv4Configuration();
        if (ipv4 == nullptr) {
            LOG_ERROR << "Can't get IPv4 configuration";
            break;
        }

        newValue.ip = ipv4->ip();

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

        auto ap = wifi->activeAccessPoint();
        if (ap != nullptr) {
            newValue.ssid = ap->ssid();
            newValue.strength = ap->strength();
        } else {
            LOG_DEBUG << "No Access point info available";
        }

        newValue.hasInternet = m_nm.state() == nm::NetworkingState::ConnectedGlobal;
    
    } while(false);

    currentNetwork.set(newValue);
}

void NetworkManager::setInterface(std::string interface)
{
    m_interface = interface;
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
