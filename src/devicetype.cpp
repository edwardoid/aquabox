#include "devicetype.h"

using namespace aquabox;

template<>
Serializable<DeviceClass>::Enum2StrMap Serializable<DeviceClass>::val2Str({
    { DeviceClass::AnalogRelay , "analog_relay" },
    { DeviceClass::Relay, "relay" },
    { DeviceClass::Sensor, "sensor" },
    { DeviceClass::Unknown, "unknown" }
});

template<>
Serializable<DeviceClass>::Str2EnumMap Serializable<DeviceClass>::str2Val({
    { "analog_relay" , DeviceClass::AnalogRelay },
    { "relay", DeviceClass::Relay},
    { "sensor", DeviceClass::Sensor},
    { "unknown", DeviceClass::Unknown }
});

template<>
Serializable<DeviceType>::Enum2StrMap Serializable<DeviceType>::val2Str({
    { DeviceType::Generic , "generic" },
    { DeviceType::Light , "light" },
    { DeviceType::AirPump, "air_pump" },
    { DeviceType::WaterPump, "water_pump" },
    { DeviceType::ExternalFilter , "external_filter" },
    { DeviceType::InternalFilter , "internal_filter" },
    { DeviceType::AirliftFilter, "airlift_filter" },
    { DeviceType::Heater , "heater" },
    { DeviceType::Cooler , "cooler" },
    { DeviceType::Feeder , "feeder" },
    { DeviceType::Dosator , "dosator" },
    { DeviceType::CO2 , "co2" },
    { DeviceType::UV , "uv" },
    { DeviceType::Thermometer , "thermometer" }
});

template<>
Serializable<DeviceType>::Str2EnumMap Serializable<DeviceType>::str2Val({
    { "generic" , DeviceType::Generic },
    { "light" , DeviceType::Light },
    { "air_pump" , DeviceType::AirPump },
    { "water_pump" , DeviceType::WaterPump },
    { "external_filter", DeviceType::ExternalFilter },
    { "internal_filter", DeviceType::InternalFilter },
    { "airlift_filter", DeviceType::AirliftFilter },
    { "heater" , DeviceType::Heater },
    { "cooler" , DeviceType::Cooler },
    { "feeder" , DeviceType::Feeder },
    { "dosator" , DeviceType::Dosator },
    { "co2" , DeviceType::CO2 },
    { "uv" , DeviceType::UV },
    { "thermometer" , DeviceType::Thermometer }
});