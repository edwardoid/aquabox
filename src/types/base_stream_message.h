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

#pragma once

#include <nlohmann/json.hpp>

namespace aquabox
{
    enum class ApiMessageType
    {
        None = 0,
        Event,
        Command,
        Error
    };

    enum class Result
    {
        Unknown = 0,
        BadCommand = 400,
        AccessDenied = 403,
        NotFound = 404,
        ServerError = 500,
        Ok = 200
    };

    struct BaseStreamMessage
    {
        BaseStreamMessage() = default;
        BaseStreamMessage(ApiMessageType type);
        ApiMessageType type;
    };

    struct BaseStreamRequest: public BaseStreamMessage
    {
        BaseStreamRequest() = default;
        inline BaseStreamRequest(ApiMessageType t): BaseStreamMessage(t) {}
        uint64_t    rqid;    
    };

    struct BaseStreamResponse: public BaseStreamMessage
    {
        BaseStreamResponse() = default;
        BaseStreamResponse(ApiMessageType t);
        uint64_t    rpid;
        std::string boxId;
    };
    
    template<typename T>
    nlohmann::json as_json(const T& o)
    {
        nlohmann::json j;
        to_json(j, o);
        return j;
    }

    void to_json(nlohmann::json& j, const ApiMessageType& type);
    void from_json(const nlohmann::json& j, ApiMessageType& type);

    void to_json(nlohmann::json& j, const Result& r);
    void from_json(const nlohmann::json& j, Result& r);


    void to_json(nlohmann::json& j, const BaseStreamMessage& msg);
    void from_json(const nlohmann::json& j, BaseStreamMessage& msg);

    void to_json(nlohmann::json& j, const BaseStreamRequest& req);
    void from_json(const nlohmann::json& j, BaseStreamRequest& req);

    void to_json(nlohmann::json& j, const BaseStreamResponse& rsp);
    void from_json(const nlohmann::json& j, BaseStreamResponse& rsp);

}