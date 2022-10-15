/*
 * AquaBox
 * Copyright (C) 2020  Eduard Sargsyan
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

#include <types/command.h>
#include <types/error_response.h>

namespace aquabox
{
    struct ExecutionResult: public BaseStreamResponse
    {
        ExecutionResult() = default;
        ExecutionResult(const ExecutionResult& src)
            : BaseStreamResponse(static_cast<const BaseStreamResponse&>(src))
        {
            this->ok = src.ok;
            this->fail = src.fail;
        }
        CommandResult ok;
        ErrorResponse fail;
        ExecutionResult& operator = (const ErrorResponse& e) { fail = e; return *this; }
        ExecutionResult& operator = (const CommandResult& r) { fail.setResult(Result::Ok); ok = r; return *this; }
        operator bool() const
        {
            return fail.result() == Result::Ok;
        }
    };
    class CommandExecutor
    {
    public:
        ExecutionResult exec(const CommandRequest& cmd);

    private:
        Result GetStatus();
        Result GetDevices();
        Result GetDevice();
        Result UpdateDevice();
        Result GetRules();
        Result GetRule();
        Result CreateOrUpdateRule(bool update);
        Result DeleteRule();
        Result DeviceRules();
        Result SetValue();
        Result ScanWiFiNetworks();
        Result ListNetworks();
        Result ConnectToWiFi();

    private:
        BaseCommand m_cmd;
        ExecutionResult m_res;
    };

    void to_json(nlohmann::json& j, const ExecutionResult& req);
} // namespace aquabox
