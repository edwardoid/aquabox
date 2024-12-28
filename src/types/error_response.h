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

#include <types/base_stream_message.h>

namespace aquabox
{
    class ErrorResponse: public BaseStreamResponse
    {
    public:
        ErrorResponse();
        ErrorResponse(Result code, const std::string& message, const std::string& details = "", int reqId = 0);

        inline void setResult(Result result) { m_result = result; }
        inline Result result() const { return m_result; }

        inline void setMessage(const std::string& message) { m_message = message; } 
        inline const std::string& message() const { return m_message; }

        inline void setDetails(const std::string& details) { m_details = details; }
        inline const std::string details() const { return m_details; }

        static ErrorResponse missedParameter(const std::string& name, int reqId = 0);
        static ErrorResponse deviceNotFound(const std::string& id, int reqId = 0);
        static ErrorResponse ruleNotFound(const std::string& id, int reqId = 0);
        static ErrorResponse wrongType(const std::string& name, const std::string& expected, const std::string& provided = "", int reqId = 0);
        static const ErrorResponse& unknownError(int reqId = 0);
        static const ErrorResponse& parseError(int reqId = 0);
        static const ErrorResponse& rqidMissed();
        static const ErrorResponse& requestIsNotCommand(int reqId = 0);
        static const ErrorResponse& boxIdMissed(int reqId = 0);
        static const ErrorResponse& authorizationFailed(int reqId = 0);
        static const ErrorResponse& boxIsOffline(int reqId = 0);
        static const ErrorResponse& appIdIdMissed(int reqId = 0);
        static const ErrorResponse& rqidWrongType();
        static const ErrorResponse& unknownCommand(int reqId = 0);
        static const ErrorResponse& unsupportedCommand(int reqId = 0);
        static const ErrorResponse& connectionRejected(int reqId = 0);
    private:
        Result m_result = Result::Ok;
        std::string m_message;
        std::string m_details;
    };

    void to_json(nlohmann::json& j, const ErrorResponse& req);
    void from_json(const nlohmann::json& j, ErrorResponse& req);
}