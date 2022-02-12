/*
 * AquaBox
 * Copyright (C) 2019  Eduard Sargsyan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "device_type.h"

using namespace aquabox;

template<>
Serializable<DeviceClass>::Enum2StrMap Serializable<DeviceClass>::val2Str({{DeviceClass::AnalogRelay, "analog_relay"},
                                                                           {DeviceClass::Relay, "relay"},
                                                                           {DeviceClass::Sensor, "sensor"},
                                                                           {DeviceClass::Step, "step"},
                                                                           {DeviceClass::Unknown, "unknown"}});

template<>
Serializable<DeviceClass>::Str2EnumMap Serializable<DeviceClass>::str2Val({{"analog_relay", DeviceClass::AnalogRelay},
                                                                           {"relay", DeviceClass::Relay},
                                                                           {"sensor", DeviceClass::Sensor},
                                                                           {"step", DeviceClass::Step},
                                                                           {"unknown", DeviceClass::Unknown}});

template<>
Serializable<DeviceType>::Enum2StrMap Serializable<DeviceType>::val2Str({{DeviceType::Generic, "generic"},
                                                                         {DeviceType::Light, "light"},
                                                                         {DeviceType::AirPump, "air_pump"},
                                                                         {DeviceType::WaterPump, "water_pump"},
                                                                         {DeviceType::ExternalFilter, "external_filter"},
                                                                         {DeviceType::InternalFilter, "internal_filter"},
                                                                         {DeviceType::AirliftFilter, "airlift_filter"},
                                                                         {DeviceType::SUMP, "sump"},
                                                                         {DeviceType::Heater, "heater"},
                                                                         {DeviceType::Cooler, "cooler"},
                                                                         {DeviceType::Feeder, "feeder"},
                                                                         {DeviceType::Dosator, "dosator"},
                                                                         {DeviceType::CO2, "co2"},
                                                                         {DeviceType::UV, "uv"},
                                                                         {DeviceType::PHController, "ph_controller"},
                                                                         {DeviceType::PHMeter, "ph_meter"},
                                                                         {DeviceType::Thermometer, "thermometer"}});

template<>
Serializable<DeviceType>::Str2EnumMap Serializable<DeviceType>::str2Val({{"generic", DeviceType::Generic},
                                                                         {"light", DeviceType::Light},
                                                                         {"air_pump", DeviceType::AirPump},
                                                                         {"water_pump", DeviceType::WaterPump},
                                                                         {"external_filter", DeviceType::ExternalFilter},
                                                                         {"internal_filter", DeviceType::InternalFilter},
                                                                         {"airlift_filter", DeviceType::AirliftFilter},
                                                                         {"sump", DeviceType::SUMP},
                                                                         {"heater", DeviceType::Heater},
                                                                         {"cooler", DeviceType::Cooler},
                                                                         {"feeder", DeviceType::Feeder},
                                                                         {"dosator", DeviceType::Dosator},
                                                                         {"co2", DeviceType::CO2},
                                                                         {"uv", DeviceType::UV},
                                                                         {"ph_controller", DeviceType::PHController},
                                                                         {"ph_meter", DeviceType::PHMeter},
                                                                         {"thermometer", DeviceType::Thermometer}});