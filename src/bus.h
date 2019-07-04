#ifndef BUS_H
#define BUS_H

#include <memory>
#include "hardware.h"

namespace aquabox
{
    class Bus
    {
    public:
        virtual std::shared_ptr<Hardware> hw(std::string hw) = 0;
    };
}

#endif // BUS_H