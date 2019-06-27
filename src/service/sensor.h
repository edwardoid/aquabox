#ifndef RELAY_H
#define RELAY_H

#include "device.h"

namespace aquabox
{
    class Sensor: public Device
    {
    public:
        Sensor() = default;
        Sensor(const Sensor& src) = default;
        Sensor(Sensor&& src) = default;

        double value() const;
    };
}

#endif // RELAY_H