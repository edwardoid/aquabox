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

#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <nlohmann/json.hpp>

namespace aquabox
{
    enum class Result
    {
        BadCommand = 400,
        AccessDenied = 403,
        NotFound = 404,
        ServerError = 500,
        Ok = 200
    };

    enum class Operation: int
    {
        Unknown = 0,
        GetStatus = 1,
        GetDevices = 2,
        GetDevice = 3,
        UpdateDevice = 4,
        GetRules = 5,
        GetRule = 6,
        CreateRule = 7,
        UpdateRule = 8,
        DeleteRule = 9,
        DeviceRules = 10,
        ScanWiFiNetworks = 11,
        WiFiNetworks = 12,
        ConnectToWiFi = 13,
        SupportedRepeatTimeouts = 14,
        TurnOn = 15,
        TurnOff = 16
    };

    class Command
    {
    public:
        Command() = default;
        Command(const Command& src);
        const Command& operator = (const Command& src);
        static bool validate(const nlohmann::json& obj);
        void setSeqNum(int num);
        int seqNum() const;
        void setBody(std::string body);
        const std::string& body() const;
        void setParameters(const std::unordered_map<std::string, std::string>& params);
        const std::unordered_map<std::string, std::string>& parameters() const;
        void setParameter(std::string name, std::string value);
        bool hasParameter(std::string name) const;
        std::string getParameter(std::string name) const;
        void setOperation(Operation op);
        Operation operation() const;
        void response(const nlohmann::json data, Result result);
        void response(Result result);
        const std::string& response() const;
        Result result() const;
        nlohmann::json toJSON() const;
        nlohmann::json toResultJSON() const;
        bool fromJSON(const nlohmann::json& obj);
    private:
        int m_seqNum = 0;
        std::unordered_map<std::string, std::string> m_params;
        std::string m_body;
        std::string m_data;
        nlohmann::json m_dataJson;
        Result m_result = Result::Ok;
        Operation m_operation;
    };
}

#endif // COMMAND_H