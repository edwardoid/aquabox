#include "physical1wbus.h"
#include "onewiredevice.h"

using namespace aquabox;

#include <ow_bus.h>
#include <unordered_map>

using namespace aquabox;

std::shared_ptr<Hardware> PhysicalOneWireBus::hw(std::string hw)
{
    static std::unordered_map<std::string, std::shared_ptr<OneWireDevice> > devices;
    if (devices.empty()) {
        for(auto& d : w1::Bus::devices()) {
            devices.insert({ d.address(), std::make_shared<OneWireDevice>(d) });
        }
    }

    auto pos = devices.find(hw);
    return pos == devices.end() ? nullptr : pos->second;
}