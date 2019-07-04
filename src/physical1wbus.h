#ifndef PHY_1W_BUS_H
#define PHY_1W_BUS_H

#include "bus.h"

namespace aquabox
{
    class PhysicalOneWireBus: public Bus
    {
    public:
        std::shared_ptr<Hardware> hw(std::string hw) override;
    };
}

#endif // PHY_1W_BUS_H