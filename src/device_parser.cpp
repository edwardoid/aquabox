#include "device_parser.h"
#include "device.h"
#include "relay.h"
#include "sensor.h"
#include "settings.h"
#include <fstream>

using namespace aquabox;

DeviceClass getClass(const Path& p)
{
    std::ifstream is(p.c_str());
    std::string content;
    std::string line;
    while(std::getline(is, line)) {
        content += line;
    }

    try {
        nlohmann::json data = nlohmann::json::parse(content);
        auto val = data.find("class").value();
        if (val.is_string()) {
            bool ok = false;
            auto clazz = Serializable<DeviceClass>::convert(val.get<std::string>(), &ok);
            return ok ? clazz : DeviceClass::Unknown;
        }
    }
    catch(std::exception e) {
        LOG_ERROR << "Can't determine device type in " << p.c_str() << " error: " << e.what();
    }


    return DeviceClass::Unknown;
}

std::unordered_map<std::string, std::shared_ptr<Device>> DeviceParser::parse(Path& root)
{
    std::unordered_map<std::string, std::shared_ptr<Device>> devices;
    for(Path p : root.children()) {
        if(p.isDirectory() || p.extension() != "json") {
            continue;
        }

        LOG_INFO << "Loading device: " << p.c_str();
        std::shared_ptr<Device> dev = nullptr;
        switch(getClass(p))
        {
            case DeviceClass::Relay: {
                dev = std::make_shared<Relay>();
                break;
            }
            case DeviceClass::Sensor: {
                dev = std::make_shared<Sensor>();
                break;
            }
            default: {
                LOG_ERROR << "Unknown device type in " << p.c_str();
            }
        }

        if(dev != nullptr && dev->parseFile(p))
            devices.insert({ dev->getId(), std::dynamic_pointer_cast<Device>(dev) });
    }
    return devices;
}