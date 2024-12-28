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

#include "error_response.h"
#include <sstream>

namespace aquabox
{
    void to_json(nlohmann::json& j, const ErrorResponse& e)
    {
        to_json(j, (const BaseStreamResponse&) e);
        try {
            j.emplace("code", e.result());
            j.emplace("message", e.message());
            if (!e.details().empty())
                j.emplace("details", e.details());

        } catch (const nlohmann::json::exception& ex) {

        }
    }

    void from_json(const nlohmann::json& j, ErrorResponse& e)
    {
        from_json(j, (BaseStreamResponse&) e);
        try {
            e.setMessage(j["message"]);
            if (j.count("details"))
                e.setDetails(j["details"]);
            e.setResult((Result) j["code"]);
        } catch (const nlohmann::json::exception& ex) {

        }
    }
}

using namespace aquabox;

ErrorResponse::ErrorResponse()
    : BaseStreamResponse(ApiMessageType::Error)
{}

ErrorResponse::ErrorResponse(Result code, const std::string& message, const std::string& details, int reqId)
    : BaseStreamResponse(ApiMessageType::Error)
{
    rpid = reqId;
    setResult(code);
    setMessage(message);
    setDetails(details);
}

ErrorResponse ErrorResponse::missedParameter(const std::string& name, int reqId)
{
    ErrorResponse err(Result::BadCommand, "Parameter is missed", name, reqId);
    return err;
}

ErrorResponse ErrorResponse::deviceNotFound(const std::string& id, int reqId)
{
    ErrorResponse err(Result::NotFound, "Device not found", id, reqId);
    return err;
}

ErrorResponse ErrorResponse::ruleNotFound(const std::string& id, int reqId)
{
    ErrorResponse err(Result::NotFound, "Rule not found", id, reqId);
    return err;
}

ErrorResponse ErrorResponse::wrongType(const std::string& name, const std::string& expected, const std::string& provided, int reqId)
{
    ErrorResponse err(Result::BadCommand, "Wrong type", "", reqId);
    std::stringstream ss;
    ss << "For parameter '" << name << "' expected type is " << expected;
    if (provided.empty())
    {
        ss << ".";
    }
    else
    {
        ss << ". Provided " << provided;
    }
    err.setDetails(ss.str());
    return err;
}

const ErrorResponse& ErrorResponse::unknownError(int reqId)
{
    static ErrorResponse err(Result::ServerError, "Unknown error", "", reqId);
    return err;
}

const ErrorResponse& ErrorResponse::parseError(int reqId)
{
    static ErrorResponse err(Result::BadCommand, "Malformed message", "", reqId);
    return err;
}

const ErrorResponse& ErrorResponse::rqidMissed()
{
    static ErrorResponse err(Result::BadCommand, "Missed rqId", "Request Id field is missed in request");
    return err;
}

const ErrorResponse& ErrorResponse::requestIsNotCommand(int reqId)
{
    static ErrorResponse err(Result::BadCommand, "Request does not contain a command", "Expected a command in request", reqId);
    return err;
}

const ErrorResponse& ErrorResponse::boxIdMissed(int reqId)
{
    static ErrorResponse err(Result::BadCommand, "Missed boxId", "Box Id fiels is missed in request", reqId);
    return err;
}

const ErrorResponse& ErrorResponse::authorizationFailed(int reqId)
{
    static ErrorResponse err(Result::AccessDenied, "Authorization faied", "This application is not authorized on specified box", reqId);
    return err;
}

const ErrorResponse& ErrorResponse::boxIsOffline(int reqId)
{
    static ErrorResponse err(Result::NotFound, "Aquabox is offline", "Specified client is not online(yet)", reqId);
    return err;
}


const ErrorResponse& ErrorResponse::appIdIdMissed(int reqId)
{
    static ErrorResponse err(Result::BadCommand, "Missed appId", "Application id is missed", reqId);
    return err;
}

const ErrorResponse& ErrorResponse::rqidWrongType()
{
    static ErrorResponse err(Result::BadCommand, "Wrong type for rqId", "Request Id must be an integer", 0);
    return err;
}

const ErrorResponse& ErrorResponse::unknownCommand(int reqId)
{
    static ErrorResponse err(Result::BadCommand, "Wrong value for command", "Specified command is unknown", reqId);
    return err;
}

const ErrorResponse& ErrorResponse::unsupportedCommand(int reqId)
{
    static ErrorResponse err(Result::BadCommand, "Unsupported command", "", reqId);
    return err;
}

const ErrorResponse& ErrorResponse::connectionRejected(int reqId)
{
    static ErrorResponse err(Result::AccessDenied, "Connection to specified box is not allowed", "", reqId);
    return err;
}