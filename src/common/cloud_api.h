#pragma once

#include <unordered_map>
#include <nlohmann/json.hpp>
/*
enum class Operation: int
{
    Unknown
};

struct BaseCloudReqResponce
{
    uint64_t sequenceId = 0;
    uint64_t inReplyTo = 0;
    ApiMessageType type;
    nlohmann::json data;
};

struct Command: public BaseCloudReqResponce
{
    Operation operation;
    std::unordered_map<std::string, std::string> parameters;
    void setParameter(std::string name, std::string value);
    bool hasParameter(std::string name) const;
    std::string getParameter(std::string name) const;
};

struct Event: public BaseCloudReqResponce
{
    std::string sender;
    std::string clazz;
    nlohmann::json properties;
};

void to_json(nlohmann::json& j, const BaseCloudReqResponce& req);
void from_json(const nlohmann::json& j, BaseCloudReqResponce& req);

void to_json(nlohmann::json& j, const Command& cmd);
void from_json(const nlohmann::json& j, Command& cmd);

void to_json(nlohmann::json& j, const Event& event);
void from_json(const nlohmann::json& j, Event& event);
*/