#ifndef DEVICE_TYPE_H
#define DEVICE_TYPE_H


#include "enumerations.h"

namespace aquabox
{
    enum class DeviceClass: int
    {
        Unknown = 0,
        Relay,
        AnalogRelay,
        Sensor
    };

    enum class DeviceType: int
    {
        Generic = 0,
        Light,
        AirPump,
        WaterPump,
        InternalFilter,
        ExternalFilter,
        AirliftFilter,
        Heater,
        Cooler,
        Feeder,
        Dosator,
        CO2,
        UV,
        Thermometer
    };
}

#endif // DEVICE_TYPE_H