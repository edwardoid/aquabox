#include "deviceparser.h"
#include "device.h"
#include "settings.h"

using namespace aquabox;

std::unordered_map<std::string, std::shared_ptr<Device>> DeviceParser::parse(Path& root)
{
    std::unordered_map<std::string, std::shared_ptr<Device>> devices;
    for(Path p : root.children())
    {
        if(p.isDirectory() || p.extension() != "json")
            continue;
        LOG_INFO << "Loading device: " << p.c_str();
        auto dev = std::make_shared<Device>();
        if(dev->parseFile(p))
            devices.insert({ dev->getId(), dev });
    }
    return devices;
}