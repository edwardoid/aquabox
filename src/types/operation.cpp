/*
 * AquaBox
 * Copyright (C) 2022  Eduard Sargsyan
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

#include "operation.h"

namespace aquabox
{

/*
template<>
Serializable<Operation>::Str2EnumMap Serializable<Operation>::str2Val({{"get_status", Operation::GetStatus},
                                                                       {"get_all_devices", Operation::GetDevices},
                                                                       {"get_device", Operation::GetDevice},
                                                                       {"update_device", Operation::UpdateDevice},
                                                                       {"get_rules", Operation::GetRules},
                                                                       {"get_rule", Operation::GetRule},
                                                                       {"create_rule", Operation::CreateRule},
                                                                       {"update_rule", Operation::UpdateRule},
                                                                       {"delete_rule", Operation::DeleteRule},
                                                                       {"get_device_rules", Operation::DeviceRules},
                                                                       {"scan", Operation::ScanWiFiNetworks},
                                                                       {"networks", Operation::WiFiNetworks},
                                                                       {"connect", Operation::ConnectToWiFi},
                                                                       {"repeat_units", Operation::SupportedRepeatTimeouts},
                                                                       {"set", Operation::Set}});

*/

void to_json(nlohmann::json& j, const Operation& o)
{
    /*
        DeleteRule = 9,
        DeviceRules = 10,
        ScanWiFiNetworks = 11,
        WiFiNetworks = 12,
        ConnectToWiFi = 13,
        SupportedRepeatTimeouts = 14,
        Set = 15,
        Call = 16,
    */
    switch (o)
    {
        case Operation::Unknown:            { j = "unknown";            break; }
        case Operation::GetStatus:          { j = "get_status";         break; }
        case Operation::GetDevices:         { j = "get_all_devices";    break; }
        case Operation::GetDevice:          { j = "get_device";         break; }
        case Operation::UpdateDevice:       { j = "update_device";      break; }
        case Operation::GetRules:           { j = "get_rules";          break; }
        case Operation::GetRule:            { j = "get_rule";           break; }
        case Operation::CreateRule:         { j = "create_rule";        break; }
        case Operation::UpdateRule:         { j = "update_rule";        break; }
        case Operation::DeleteRule:         { j = "delete_rule";        break; }
        case Operation::DeviceRules:        { j = "get_device_rules";   break; }
        case Operation::ScanWiFiNetworks:   { j = "scan";               break; }
        case Operation::WiFiNetworks:       { j = "networks";           break; }
        case Operation::ConnectToWiFi:      { j = "connect";            break; }
        case Operation::Set:                { j = "set";                break; }
        case Operation::Call:               { j = "call";               break; }
        case Operation::SubscribeToBox:     { j = "subsribe_to_box";    break; }
    }
}

void from_json(const nlohmann::json& j, Operation& o)
{
    try
    {
        std::string v = j.get<std::string>();
        static std::unordered_map<std::string, Operation> values({
            {"get_status", Operation::GetStatus},
            {"get_all_devices", Operation::GetDevices},
            {"get_device", Operation::GetDevice},
            {"update_device", Operation::UpdateDevice},
            {"get_rules", Operation::GetRules},
            {"get_rule", Operation::GetRule},
            {"create_rule", Operation::CreateRule},
            {"update_rule", Operation::UpdateRule},
            {"delete_rule", Operation::DeleteRule},
            {"get_device_rules", Operation::DeviceRules},
            {"scan", Operation::ScanWiFiNetworks},
            {"networks", Operation::WiFiNetworks},
            {"connect", Operation::ConnectToWiFi},
            {"set", Operation::Set},
            {"subsribe_to_box", Operation::SubscribeToBox}
        });
        o = values.at(v);
    } catch (const std::out_of_range& ex) {

    }
}

}