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

    enum class Operation
    {
        GetStatus = 0,
        GetDevices = 1,
        GetDevice = 2,
        UpdateDevice = 3,
        GetRules = 4,
        GetRule = 5,
        CreateRule = 6,
        UpdateRule = 7,
        DeleteRule = 8,
        DeviceRules = 9,
        ScanWiFiNetworks = 10,
        ConnectToWiFi = 11,
        SupportedRepeatTimeouts = 12,
        TurnOn = 13,
        TurnOff = 14
    };

    class Command
    {
    public:
        void setBody(std::string body);
        const std::string& body() const;
        void setParameters(const std::unordered_map<std::string, std::string>& params);
        void setParameter(std::string name, std::string value);
        bool hasParameter(std::string name) const;
        std::string getParameter(std::string name) const;
        void setOperation(Operation op);
        Operation operation() const;
        void response(const nlohmann::json data, Result result);
        void response(Result result);
        const std::string& response() const;
        Result result() const;
    private:
        std::unordered_map<std::string, std::string> m_params;
        std::string m_body;
        std::string m_data;
        Result m_result;
        Operation m_operation;
    };
}

#endif // COMMAND_H