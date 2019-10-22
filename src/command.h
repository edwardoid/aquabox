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
        ConnectToWiFi = 12,
        SupportedRepeatTimeouts = 13,
        TurnOn = 14,
        TurnOff = 15
    };

    class Command
    {
    public:
        void setSeqNum(int num);
        int seqNum() const;
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
        int m_seqNum = 0;
        std::unordered_map<std::string, std::string> m_params;
        std::string m_body;
        std::string m_data;
        Result m_result;
        Operation m_operation;
    };
}

#endif // COMMAND_H