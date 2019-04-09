#include "deviceparser.h"
#include "device.h"
#include "settings.h"

using namespace aquabox;
std::unordered_map<std::string, std::shared_ptr<Device>> DeviceParser::parse()
{
    std::unordered_map<std::string, std::shared_ptr<Device>> devices;
    auto devs = Settings::array(Settings::DEVICES);
    
    LOG_INFO << "Loading " << devs.size() << " devices...";
    for (auto d : devs)
    {
        auto dev = std::make_shared<Device>();
        if(dev->parse(d))
            devices.insert({ dev->getId().Value, dev});
    }

    return devices;
}