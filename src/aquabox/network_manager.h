#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <string>
#include <vector>
#include <s2s_property.h>

namespace aquabox
{
    enum class Authentication
    {
        None,
        WEP,
        WPA,
        WPA2,
        Enterprise
    };

    struct WifiNetwork
    {
        std::string ssid;
        std::string password;
        std::string ip;
        Authentication auth;
        bool encrypted = 0;
        int signal = 100;

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

    class NetworkManager
    {
        NetworkManager();
        ~NetworkManager();
    public:

        static NetworkManager& i();
        bool connectedToInternet() { return false; }
        void setInterface(std::string interface);
        bool requestScan(bool force = true);
        SignalSlot::Property<WifiNetwork> currentNetwork;
        std::vector<WifiNetwork> networks();
        bool connect(WifiNetwork network);
        SignalSlot::Signal<size_t> NetworksChanged;
        void updateCurrentNetwork();
    private:
        struct Data* m_data;
    };
}

#endif // NETWORK_MANAGER_H