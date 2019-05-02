#include "networkmanager.h"
#include <pstream.h>
#include <sstream>

using namespace aquabox;

bool InterfaceInfo::parse(std::string cli)
{
    std::istringstream ss(cli);
    std::string connectedStr;
    ss >> name >> device >> connectedStr >>  connection;
    connected = connectedStr == "connected";
    ip = NetworkManager::ip(name);
    return true;
}

bool WifiInfo::parse(std::string cli)
{
    std::istringstream ss(cli);
    std::string connectedStr;
    std::string mode, channel, speed;
    int rate;
    ss >> SSID;
    if (SSID == "*")
        ss >> SSID;
    ss >> mode >> channel >> rate >> speed >> strength >> bars;
    return true;
}


std::vector<InterfaceInfo> NetworkManager::interfaces()
{
    std::vector<InterfaceInfo> interfaces;
    redi::ipstream ps("nmcli -c no d");
    bool lineNum = 0;
    std::string line;
    while(std::getline(ps, line)) {
        if (lineNum++ == 0)
            continue;
        InterfaceInfo info;
        if(info.parse(line)) {
            if (info.device == "ethernet" || info.device == "wifi")
                interfaces.push_back(info);
        }
    }

    return interfaces;
}

std::string NetworkManager::ip(std::string interface)
{
    std::string cmd = "nmcli -s -c no device show ";
    cmd += interface;
    cmd += " | grep IP4.ADD | egrep -o '[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+' --color=never";

    redi::ipstream ps(cmd);
    std::string line;
    ps >> line;
    return line;
}

std::vector<WifiInfo> NetworkManager::wifiNetworks()
{
    std::vector<WifiInfo> wifis;
    redi::ipstream ps("nmcli --show-secrets -c no dev wifi list |  awk -F'  +' '{ if (!seen[$2]++) print}'");
    bool lineNum = 0;
    std::string line;
    while(std::getline(ps, line)) {
        if (lineNum++ == 0)
            continue;
        WifiInfo info;
        if(info.parse(line)) {
            wifis.push_back(info);
        }
    }

    return wifis;
}