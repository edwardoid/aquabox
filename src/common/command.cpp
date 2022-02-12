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
#include "command_schema.h"
#include "enumerations.h"
#include "validation_error_handler.h"

using namespace aquabox;

template<>
Serializable<Operation>::Enum2StrMap Serializable<Operation>::val2Str({{Operation::GetStatus, "get_status"},
                                                                       {Operation::GetDevices, "get_all_devices"},
                                                                       {Operation::GetDevice, "get_device"},
                                                                       {Operation::UpdateDevice, "update_device"},
                                                                       {Operation::GetRules, "get_rules"},
                                                                       {Operation::GetRule, "get_rule"},
                                                                       {Operation::CreateRule, "create_rule"},
                                                                       {Operation::UpdateRule, "update_rule"},
                                                                       {Operation::DeleteRule, "delete_rule"},
                                                                       {Operation::DeviceRules, "get_device_rules"},
                                                                       {Operation::ScanWiFiNetworks, "scan"},
                                                                       {Operation::ConnectToWiFi, "connect"},
                                                                       {Operation::WiFiNetworks, "networks"},
                                                                       {Operation::SupportedRepeatTimeouts, "repeat_units"},
                                                                       {Operation::Set, "set"}});

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

template<>
Serializable<Result>::Enum2StrMap Serializable<Result>::val2Str({{Result::BadCommand, "bad_command"},
                                                                 {Result::AccessDenied, "access_denied"},
                                                                 {Result::NotFound, "not_found"},
                                                                 {Result::ServerError, "server_error"},
                                                                 {Result::Ok, "ok"}});

template<>
Serializable<Result>::Str2EnumMap Serializable<Result>::str2Val({{"bad_command", Result::BadCommand},
                                                                 {"access_denied", Result::AccessDenied},
                                                                 {"not_found", Result::NotFound},
                                                                 {"server_error", Result::ServerError},
                                                                 {"ok", Result::Ok}});

Command::Command(const Command& src)
    : m_seqNum(src.m_seqNum)
    , m_params(src.m_params)
    , m_request(src.m_request)
    , m_response(src.m_response)
    , m_result(src.m_result)
    , m_operation(src.m_operation)
{}

const Command& Command::operator=(const Command& src)
{
    m_seqNum = src.m_seqNum;
    m_params = src.m_params;
    m_request = src.m_request;
    m_response = src.m_response;
    m_result = src.m_result;
    m_operation = src.m_operation;

    return *this;
}

bool Command::validate(const nlohmann::json& obj)
{
    ValidationErrorHandler handler;
    CommandValidator::validator().validate(obj, handler);
    return handler.error().empty();
}

void Command::setSeqNum(int num)
{
    m_seqNum = num;
}

int Command::seqNum() const
{
    return m_seqNum;
}

void Command::setRequest(const nlohmann::json& request)
{
    m_request = request;
}

const nlohmann::json& Command::request() const
{
    return m_request;
}

void Command::setParameters(const std::unordered_map<std::string, std::string>& params)
{
    for (auto p : params) {
        m_params.insert({p.first, p.second});
    }
}

const std::unordered_map<std::string, std::string>& Command::parameters() const
{
    return m_params;
}

void Command::setParameter(std::string name, std::string value)
{
    m_params.insert({name, value});
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

void Command::response(const nlohmann::json& response, Result result)
{
    m_response = response;
    m_result = result;
}

void Command::response(Result result)
{
    static const std::string NoData("");
    response(NoData, result);
}

const nlohmann::json& Command::response() const
{
    return m_response;
}

Result Command::result() const
{
    return m_result;
}

nlohmann::json Command::responseJson() const
{
    nlohmann::json obj({});

    obj["seqNum"] = m_seqNum;
    obj["params"] = m_params;
    obj["operation"] = Serializable<Operation>::convert(m_operation);
    obj["result"] = Serializable<Result>::convert(m_result);
    if (!m_response.is_null())
        obj["response"] = m_response;
    return obj;
}

bool Command::fromJSON(const nlohmann::json& obj)
{
    m_seqNum = obj["seqNum"];
    m_operation = Serializable<Operation>::convert(obj["operation"].get<std::string>());
    if (obj.find("result") != obj.cend()) {
        m_result = Serializable<Result>::convert(obj["result"].get<std::string>());
    }

    if (obj.find("response") != obj.cend()) {
        m_response = obj["response"];
    }

    if (obj.find("request") != obj.cend()) {
        m_request = obj["request"];
    }

    if (obj.find("params") != obj.cend()) {
        for (const auto entry : obj["params"].items()) {
            std::string key = (std::string)(entry.key());
            std::string value = (std::string)(entry.value());
            m_params.insert(std::pair<std::string, std::string>(key, value));
        }
    }

    return true;
}
