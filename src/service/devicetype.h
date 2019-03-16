#ifndef DEVICE_TYPE_H
#define DEVICE_TYPE_H

namespace aquabox
{
    enum class DeviceClass
    {
        Unknown = 0,
        Relay,
        AnalogRelay
    };

    enum class DeviceType
    {
        Generic = 0,
        Light,
        Compressor,
        Filter,
        Heater,
        CO2
    };
}

#endif // DEVICE_TYPE_H