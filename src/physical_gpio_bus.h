#ifndef PHY_GPIO_BUS_H
#define PHY_GPIO_BUS_H

#include "bus.h"

namespace aquabox
{
    class PhysicalGPIOBus: public Bus
    {
    public:
        std::shared_ptr<Hardware> hw(std::string hw) override;
    };
}

#endif // PHY_GPIO_BUS_H