#include "physical1wbus.h"
#include "onewiredevice.h"
#include "log.h"

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
    if (pos == devices.end()) {
        LOG_ERROR << "Can't find 1-Wire device with adress: " << hw;
    }
    return pos == devices.end() ? nullptr : pos->second;
}