#ifndef MOCK_GPIO_BUS_H
#define MOCK_GPIO_BUS_H

#include "bus.h"

namespace aquabox
{
    class MockGPIOBus: public Bus
    {
    public:
        std::shared_ptr<Hardware> hw(std::string hw) override;
    };
}

#endif // MOCK_GPIO_BUS_H