#include "mock_gpio_bus.h"
#include "mock_hardware.h"
#include <unordered_map>

using namespace aquabox;

std::shared_ptr<Hardware> MockGPIOBus::hw(std::string hw)
{
    static std::unordered_map<std::string, std::shared_ptr<MockHardware> > pins({
        { "12", nullptr },
        { "11", nullptr },
        { "6", nullptr },
        { "1", nullptr },
        { "0", nullptr },
        { "3", nullptr },
        { "15", nullptr },
        { "16", nullptr },
        { "14", nullptr },
        { "198", nullptr },
        { "199", nullptr },
        { "7", nullptr },
        { "19", nullptr },
        { "18", nullptr },
        { "2", nullptr },
        { "13", nullptr },
        { "10", nullptr }
    });

    auto pos = pins.find(hw);
    if (pos == pins.end())
        return nullptr;
    if (pos->second == nullptr)
        pos->second = std::make_shared<MockHardware>(std::string("GPIO_pin") + pos->first);

    return pos->second;
}