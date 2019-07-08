#include "physical_layer.h"
#include "physical_gpio_bus.h"
#include "physical_1_wire_bus.h"
#include <unordered_map>

using namespace aquabox;

std::shared_ptr<Bus> Physical::bus(std::string bus)
{
    static std::unordered_map<std::string, std::shared_ptr<Bus> > availableBuses({
        { "GPIO", std::make_shared<PhysicalGPIOBus>() },
        { "1W", std::make_shared<PhysicalOneWireBus>() }
    });

    auto p = availableBuses.find(bus);
    return p == availableBuses.end() ? nullptr : p->second;
}