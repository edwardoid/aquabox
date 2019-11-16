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

#include "command.h"
#include "enumerations.h"

using namespace aquabox;

template<>
Serializable<Operation>::Enum2StrMap Serializable<Operation>::val2Str({
    { Operation::GetStatus, "get_status" },
    { Operation::GetDevices, "get_all_devices" },
    { Operation::GetDevice, "get_device" },
    { Operation::UpdateDevice, "update_device" },
    { Operation::GetRules, "get_rules" },
    { Operation::GetRule, "get_rule" },
    { Operation::CreateRule, "create_rule" },
    { Operation::UpdateRule, "update_rule" },
    { Operation::DeleteRule, "delete_rule" },
    { Operation::DeviceRules, "get_device_rules" },
    { Operation::ScanWiFiNetworks, "scan" },
    { Operation::ConnectToWiFi, "connect" },
    { Operation::SupportedRepeatTimeouts, "repeat_units" },
    { Operation::TurnOn, "turn_on" },
    { Operation::TurnOff, "turn_off" }
});

template<>
Serializable<Operation>::Str2EnumMap Serializable<Operation>::str2Val({
    { "get_status", Operation::GetStatus },
    { "get_all_devices", Operation::GetDevices },
    { "get_device", Operation::GetDevice },
    { "update_device", Operation::UpdateDevice },
    { "get_rules", Operation::GetRules },
    { "get_rule", Operation::GetRule },
    { "create_rule", Operation::CreateRule },
    { "update_rule", Operation::UpdateRule },
    { "delete_rule", Operation::DeleteRule },
    { "get_device_rules", Operation::DeviceRules },
    { "scan", Operation::ScanWiFiNetworks },
    { "connect", Operation::ConnectToWiFi },
    { "repeat_units", Operation::SupportedRepeatTimeouts },
    { "turn_on", Operation::TurnOn },
    { "turn_off", Operation::TurnOff }
});

void Command::setSeqNum(int num)
{
    m_seqNum = num;
}


int Command::seqNum() const
{
    return m_seqNum;
}

void Command::setBody(std::string body)
{
    m_body = body;
}

const std::string& Command::body() const
{
    return m_body;
}

void Command::setParameters(const std::unordered_map<std::string, std::string>& params)
{
    m_params = params;
}

void Command::setParameter(std::string name, std::string value)
{
    m_params.insert({ name, value });
}

bool Command::hasParameter(std::string name) const
{
    auto pos = m_params.find(name);
    return pos != m_params.end() && !pos->second.empty();
}

std::string Command::getParameter(std::string name) const
{
    auto p = m_params.find(name);
    if (p == m_params.end()) {
        return "";
    }
    return p->second;
}

void Command::setOperation(Operation op)
{
    m_operation = op;
}

Operation Command::operation() const
{
    return m_operation;
}

void Command::response(const nlohmann::json data, Result result)
{
    m_data = data.dump(4);
    m_result = result;
}

void Command::response(Result result)
{
    static const std::string NoData("");
    response(NoData, result);
}

const std::string& Command::response() const
{
    return m_data;
}

Result Command::result() const
{
    return m_result;
}
