#include "wifi.h"
#include <unordered_map>
#include <iwlib.h>

using namespace aquabox;

std::string Network::qr() const
{
    std::string str = "WIFI:";

    str += "T:";
    switch (Auth)
    {
        case Authentication::None:
            str += "nopass";
            break;
        case Authentication::WEP:
            str += "WEP";
            break;
        case Authentication::WPA:
            str += "WPA";
            break;
        default:
            break;
    }
    str += ";S:" + ESSID;
    str += ";P:" + Password;

    str += Hidden ? ";H" : ";";
    return str;
}

WiFi::WiFi(const std::string interface)
    : m_interface(interface)
{

}

std::vector<Network> WiFi::scan(bool all)
{
    std::vector<Network> availableNetworks;
    iwrange range;
    
    int sock = iw_sockets_open();

    /* Get some metadata to use for scanning */
    if (iw_get_range_info(sock, m_interface.c_str(), &range) < 0)
    {
        printf("Error during iw_get_range_info. Aborting.\n");
        return availableNetworks;
    }

    wireless_scan_head head;
    /* Perform the scan */
    if (iw_scan(sock, const_cast<char*>(m_interface.c_str()), range.we_version_compiled, &head) < 0) {
        printf("Error during iw_scan. Aborting.\n");
        return availableNetworks;
    }


    std::unordered_map<std::string, Network> allNetworks;
    /* Traverse the results */
    wireless_scan *result = head.result;
    while (NULL != result) {
        Network nw;
        nw.ESSID = result->b.essid;
        nw.Quality = (static_cast<double>(result->stats.qual.qual) / static_cast<double>(range.max_qual.qual)) * 100;
        if (nw.ESSID.empty())
            continue;
        if (all)
            availableNetworks.push_back(nw);
        else
        {
            auto it = allNetworks.find(nw.ESSID);
            if (it != allNetworks.cend() && it->second.Quality < nw.Quality)
                it->second = nw;
            else
                allNetworks.insert({ nw.ESSID, nw });
        }
        
        result = result->next;
    }

    if (!all)
    {
        for (auto& nw : allNetworks)
        {
            availableNetworks.push_back(nw.second);
        }
    }

    return availableNetworks;
}