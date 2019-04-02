#include "devicetype.h"

using namespace aquabox;

template<>
Serializable<DeviceClass>::Enum2StrMap Serializable<DeviceClass>::val2Str({
    { DeviceClass::AnalogRelay , "analog_relay" },
    { DeviceClass::Relay, "relay" },
    { DeviceClass::Unknown, "unknown" }
});

template<>
Serializable<DeviceClass>::Str2EnumMap Serializable<DeviceClass>::str2Val({
    { "analog_relay" , DeviceClass::AnalogRelay },
    { "relay", DeviceClass::Relay},
    { "unknown", DeviceClass::Unknown }
});

template<>
Serializable<DeviceType>::Enum2StrMap Serializable<DeviceType>::val2Str({
    { DeviceType::Generic , "generic" },
    { DeviceType::Light , "light" },
    { DeviceType::AirPump, "air_pump" },
    { DeviceType::WaterPump, "water_pump" },
    { DeviceType::Filter , "filter" },
    { DeviceType::Heater , "heater" },
    { DeviceType::Feeder , "feeder" },
    { DeviceType::CO2 , "co2" },
    { DeviceType::UV , "uv" }
});

template<>
Serializable<DeviceType>::Str2EnumMap Serializable<DeviceType>::str2Val({
    { "generic" , DeviceType::Generic },
    { "light" , DeviceType::Light },
    { "air_pump" , DeviceType::AirPump },
    { "water_pump" , DeviceType::WaterPump },
    { "filter" , DeviceType::Filter },
    { "heater" , DeviceType::Heater },
    { "feeder" , DeviceType::Feeder },
    { "co2" , DeviceType::CO2 },
    { "uv" , DeviceType::UV }
});