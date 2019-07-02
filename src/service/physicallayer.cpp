#include "physicallayer.h"
#include "physicalgpiobus.h"
#include "physical1wbus.h"
#include <unordered_map>

using namespace aquabox;

std::shared_ptr<Bus> Physical::bus(std::string bus)
{
    static std::unordered_map<std::string, std::shared_ptr<Bus> > availableBuses({
        { "GPIO", std::make_shared<PhysicalGPIOBus>() },
        { "W1", std::make_shared<PhysicalOneWireBus>() }
    });

    auto p = availableBuses.find(bus);
    return p == availableBuses.end() ? nullptr : p->second;
}