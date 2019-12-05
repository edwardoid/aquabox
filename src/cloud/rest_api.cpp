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

#include "rest_api.h"
#include "log.h"
#include "command.h"
#include "command_executor.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdio.h>
#include <random>

using namespace aquabox::v1;
using namespace aquabox;

Command createCommand(Operation operation, API::Request request, bool body, std::unordered_map<std::string, std::string> params = {})
{
    Command command;
    command.setOperation(operation);
    command.setParameters(params);
    if (body) {
        command.setBody(request->content.string());
    }
    return command;
}

void execCommand(Command& cmd, API::Response response, API::Headers headers)
{
    CommandExecutor exec;
    if(!exec.exec(cmd)) {
        LOG_ERROR << "Can't execute command! " << (int)(cmd.operation());
    } else {
        LOG_DEBUG << "Command " << (int)(cmd.operation()) << " finished with " << (int)(cmd.result()) << " data: " << cmd.response();
    }
    SimpleWeb::StatusCode code = SimpleWeb::StatusCode::server_error_not_implemented;
    switch (cmd.result())
    {
    case Result::BadCommand: { code =  SimpleWeb::StatusCode::client_error_bad_request; break; }
    case Result::AccessDenied: { code =  SimpleWeb::StatusCode::client_error_forbidden; break; }
    case Result::NotFound: { code =  SimpleWeb::StatusCode::client_error_not_found; break; }
    case Result::ServerError: { code =  SimpleWeb::StatusCode::server_error_internal_server_error; break; }
    case Result::Ok: { code =  SimpleWeb::StatusCode::success_ok; break; }
    
    default:
        code =  SimpleWeb::StatusCode::success_ok;
    }

    if (cmd.response().empty()) {
        response->write(code, headers);
    } else {
        response->write(code, cmd.response(), headers);
    }
}

void REST::init(std::shared_ptr<HttpServer> http, Route base)
{
    API::init(http, base);
}

void REST::Devices(const Route& route, API::Request& request, API::Response& response, Headers& headers)
{
    auto cmd = createCommand(Operation::GetDevices, request, false);
    execCommand(cmd, response, headers);
}

void REST::GetDevice(const Route& route, API::Request& request, API::Response& response, Headers& headers)
{
    auto cmd = createCommand(Operation::GetDevice, request, false, route.extract(request->path));
    execCommand(cmd, response, headers);
}

void REST::UpdateDevice(const Route& route, API::Request& request, API::Response& response, Headers& headers)
{
    auto cmd = createCommand(Operation::UpdateDevice, request, true, route.extract(request->path));
    execCommand(cmd, response, headers);
}

void REST::Rules(const Route& route, API::Request& request, API::Response& response, Headers& headers)
{
    auto cmd = createCommand(Operation::GetRules, request, false);
    execCommand(cmd, response, headers);
}

void REST::GetRule(const Route& route, API::Request& request, API::Response& response, Headers& headers)
{
    auto cmd = createCommand(Operation::GetRules, request, false, route.extract(request->path));
    execCommand(cmd, response, headers);
}

void REST::CreateRule(const Route& route, API::Request& request, API::Response& response, Headers& headers)
{
    auto cmd = createCommand(request->method == "PUT"
                    ? Operation::CreateRule
                    : Operation::UpdateRule, request, true, route.extract(request->path));
    execCommand(cmd, response, headers);
}

void REST::DeleteRule(const Route& route, API::Request& request, API::Response& response, Headers& headers)
{
    auto cmd = createCommand(Operation::DeleteRule, request, false, route.extract(request->path));
    execCommand(cmd, response, headers);
}

void REST::DeviceRules(const Route& route, API::Request& request, API::Response& response, Headers& headers)
{
    auto cmd = createCommand(Operation::DeviceRules, request, false, route.extract(request->path));
    execCommand(cmd, response, headers);
}

void REST::SupportedRepeatTimeouts(const Route& route, API::Request& request, API::Response& response, Headers& headers)
{
    auto cmd = createCommand(Operation::SupportedRepeatTimeouts, request, false);
    execCommand(cmd, response, headers);
}

void REST::TurnOn(const Route& route, API::Request& request, API::Response& response, Headers& headers)
{
    auto cmd = createCommand(Operation::TurnOn, request, false, route.extract(request->path));
    execCommand(cmd, response, headers);
}

void REST::TurnOff(const Route& route, API::Request& request, API::Response& response, Headers& headers)
{
    auto cmd = createCommand(Operation::TurnOff, request, false, route.extract(request->path));
    execCommand(cmd, response, headers);
}

void REST::ScanWiFiNetworks(const Route& route, API::Request& request, API::Response& response, Headers& headers)
{
    auto cmd = createCommand(Operation::ScanWiFiNetworks, request, false);
    execCommand(cmd, response, headers);
}

void REST::ConnectToWiFi(const Route& route, API::Request& request, API::Response& response, Headers& headers)
{
    Command cmd = createCommand(Operation::ConnectToWiFi, request, true);
    execCommand(cmd, response, headers);
}