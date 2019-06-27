#include "deviceparser.h"
#include "device.h"
#include "relay.h"
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
        auto dev = std::make_shared<Relay>();
        if(dev->parseFile(p))
            devices.insert({ dev->getId(), std::dynamic_pointer_cast<Device>(dev) });
    }
    return devices;
}