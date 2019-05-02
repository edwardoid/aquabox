#ifndef WIFI_NETWORK
#define WIFI_NETWORK

#include <string>
#include <vector>

namespace aquabox
{
    enum class Authentication
    {
        None,
        WEP,
        WPA
    };

    struct Network
    {
        std::string qr() const;
        std::string ESSID;
        std::string Password;
        int Quality = 100;
        Authentication Auth; 
        bool Hidden = false;
    };

    class WiFi
    {
    public:
        WiFi(const std::string interface);
        std::vector<Network> scan(bool all = false);
    private:
        std::string m_interface;
    };
}

#endif // WIFI_NETWORK
