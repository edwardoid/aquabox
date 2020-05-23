#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <string>
#include <vector>
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
        std::vector<WifiNetwork> networks();
        bool connect(const std::string& ssid, const std::string& password);
        bool activateConnection(std::string uuid);
        SignalSlot::Signal<size_t> NetworksChanged;
        void updateCurrentState();
    private:
        std::string m_interface;
        nm::NetworkManager m_nm;
    };
}

#endif // NETWORK_MANAGER_H