#include "cloud_api.h"

/*

void to_json(nlohmann::json& j, const BaseCloudReqResponce& req)
{
    if (!j.is_object()) {
        j = nlohmann::json::object();
    }
    try {
        j.emplace("sequenceId", req.sequenceId);
        j.emplace("inReplyTo", req.inReplyTo);
        j.emplace("type", req.type);
        j.emplace("data", req.data);
    } catch (const nlohmann::json::exception& ex) {

    }
}
void from_json(const nlohmann::json& j, BaseCloudReqResponce& req)
{
    try {
        req.sequenceId = (uint64_t) j["sequenceId"];
        req.inReplyTo = (uint64_t) j["sequenceId"];
        req.type = (ApiMessageType) j["sequenceId"];
    } catch (const nlohmann::json::exception& ex) {

    }
}

void to_json(nlohmann::json& j, const Command& cmd)
{
    to_json(j, (const BaseCloudReqResponce&) cmd);
    try {
        j.emplace("operation", cmd.operation);
    } catch (const nlohmann::json::exception& ex) {

    }
}

void from_json(const nlohmann::json& j, Command& cmd)
{
    from_json(j, (BaseCloudReqResponce&) cmd);
    try {
        cmd.operation = j["operation"];
    } catch (const nlohmann::json::exception& ex) {

    }
}

*/