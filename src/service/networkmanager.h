#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <vector>
#include <string>

namespace aquabox
{
    struct InterfaceInfo
    {
        bool parse(std::string cli);
        std::string name;
        std::string device;
        bool connected;
        std::string connection;
        std::string ip;
    };

    struct WifiInfo
    {
        bool parse(std::string cli);
        std::string SSID;
        int strength;
        std::string bars;
    };

    class NetworkManager
    {
    public:
        static std::vector<InterfaceInfo> interfaces();
        static std::string ip(std::string interface);
        static std::vector<WifiInfo> wifiNetworks();
    };
}

#endif // NETWORK_MANAGER_H