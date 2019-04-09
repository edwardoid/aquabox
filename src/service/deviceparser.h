#ifndef DEVICE_PARSER_H
#define DEVICE_PARSER_H

#include <string>
#include <unordered_map>
#include <memory>

namespace aquabox
{
    class Device;

    class DeviceParser
    {
        public:
        static std::unordered_map<std::string, std::shared_ptr<Device>> parse();
    };
}

#endif // DEVICE_PARSER_H