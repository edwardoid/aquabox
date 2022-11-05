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

#include "base_stream_message.h"
#include "serial.h"


namespace aquabox {
    void to_json(nlohmann::json& j, const ApiMessageType& type)
    {
        if (type == ApiMessageType::Event) j = "event";
        else if (type == ApiMessageType::Command) j = "command";
        else if (type == ApiMessageType::Error) j = "error";
        else j = "none";
    }

    void from_json(const nlohmann::json& j, ApiMessageType& type)
    {
        if (!j.is_string()) return;

        std::string t = j;
        if (t == "event") type = ApiMessageType::Event;
        else if (t == "command") type = ApiMessageType::Command;
        else if (t == "error") type = ApiMessageType::Error;
        else type = ApiMessageType::None;
    }

    void to_json(nlohmann::json& j, const Result& r)
    {
        switch (r)
        {
            case Result::Unknown:     { j = "unknown";        break; }
            case Result::BadCommand:  { j = "bad_command";    break; }
            case Result::AccessDenied:{ j = "access_denied";  break; }
            case Result::NotFound:    { j = "not_found";      break; }
            case Result::ServerError: { j = "server_error";   break; }
            case Result::Ok:          { j = "ok";             break; }
        }
    }

    void from_json(const nlohmann::json& j, Result& o)
    {
        try
        {
            std::string v = j.get<std::string>();
            static std::unordered_map<std::string, Result> values({
                {"unknown", Result::Unknown},
                {"bad_command", Result::BadCommand},
                {"access_denied", Result::AccessDenied},
                {"not_found", Result::NotFound},
                {"server_error", Result::ServerError},
                {"ok", Result::Ok}});
            o = values.at(v);
        } catch (const std::out_of_range& ex) {

        }
    }

    void to_json(nlohmann::json& j, const BaseStreamMessage& msg)
    {
        if (!j.is_object()) {
            j = nlohmann::json::object();
        }
        try {
            j.emplace("type", msg.type);
        } catch (const nlohmann::json::exception& ex) {

        }
    }
    void from_json(const nlohmann::json& j, BaseStreamMessage& msg)
    {
        try {
            msg.type = (ApiMessageType) j["type"];
        } catch (const nlohmann::json::exception& ex) {

        }
    }

    void to_json(nlohmann::json& j, const BaseStreamRequest& req)
    {
        if (!j.is_object()) {
            j = nlohmann::json::object();
        }
        try {
            to_json(j, static_cast<const BaseStreamMessage&>(req));
            j.emplace("rqid", req.rqid);
        } catch (const nlohmann::json::exception& ex) {

        }
    }

    void from_json(const nlohmann::json& j, BaseStreamRequest& req)
    {
        try {
            from_json(j, static_cast<BaseStreamMessage&>(req));
            req.rqid = j["rqid"];
        } catch (const nlohmann::json::exception& ex) {

        }
    }

    void to_json(nlohmann::json& j, const BaseStreamResponse& resp)
    {
        if (!j.is_object()) {
            j = nlohmann::json::object();
        }
        try {
            to_json(j, static_cast<const BaseStreamMessage&>(resp));
            j.emplace("boxId", resp.boxId);
            j.emplace("rpid", resp.rpid);
        } catch (const nlohmann::json::exception& ex) {

        }
    }
    
    void from_json(const nlohmann::json& j, BaseStreamResponse& resp)
    {
        try {
            from_json(j, static_cast<BaseStreamMessage&>(resp));
            resp.rpid = j["rpid"];
            resp.boxId = j["boxId"];
        } catch (const nlohmann::json::exception& ex) {

        }
    }
}



using namespace aquabox;


BaseStreamMessage::BaseStreamMessage(ApiMessageType t)
    : type(t)
{}

BaseStreamResponse::BaseStreamResponse(ApiMessageType t): BaseStreamMessage(t)
{
    boxId = serial();
}
