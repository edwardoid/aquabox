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
#include "boxes_manager.h"
#include "box.h"
#include <random>
#include <atomic>

using namespace aquabox::v1;
using namespace aquabox;

Command createCommand(Operation operation, Web::Request request, bool body, std::unordered_map<std::string, std::string> params = std::unordered_map<std::string, std::string>())
{
    static std::atomic_int seqNum;
    Command command;
    command.setSeqNum(seqNum.fetch_add(1));
    command.setOperation(operation);
    auto boxId = request->header.find("boxId");
    if (boxId != request->header.end()) {
        command.setParameter(boxId->first, boxId->second);
    }
    command.setParameters(params);
    if (body) {
        try {
            command.setRequest(nlohmann::json::parse(request->content.string()));
        }
        catch (...) {
            LOG_ERROR << "Can't set request data for operation " << (int) operation;
        }
    }
    return command;
}

void execCommand(Command& cmd, Web::Response response, Web::Headers headers)
{
    if (!cmd.hasParameter("boxId")) {
        response->write(SimpleWeb::StatusCode::client_error_bad_request, "Missed BoxId");
        return;
    }

    auto box = BoxesManager::get(cmd.getParameter("boxId"), true);
    if (box == nullptr) {
        response->write(SimpleWeb::StatusCode::client_error_not_found, "Specified box is not available");
        return; 
    }

    if (box->empty()) {
        response->write(nlohmann::json({ { "online", false } }).dump());
        return;
    }

    box->runCommand(cmd);

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

    if (cmd.response().is_null()) {
        response->write(code, headers);
    } else {
        response->write(code, cmd.response().dump(4), headers);
    }
}

bool REST::IsAuthorized(const Route& route, Request& request, Response& response, Web::Headers& headers)
{
    auto params = route.extract(request->path);
    auto appId = params.find(":appId");
    auto boxId = params.find(":boxId");
    if (appId == params.end() || boxId == params.end()) {
        return false;
    }
    return false;
}

void REST::Status(const Route& route, Web::Request& request, Web::Response& response, Web::Headers& headers)
{
    auto cmd = createCommand(Operation::GetStatus, request, false);
    execCommand(cmd, response, headers);
}

void REST::Devices(const Route& route, Web::Request& request, Web::Response& response, Web::Headers& headers)
{
    auto cmd = createCommand(Operation::GetDevices, request, false);
    execCommand(cmd, response, headers);
}

void REST::GetDevice(const Route& route, Web::Request& request, Web::Response& response, Web::Headers& headers)
{
    auto cmd = createCommand(Operation::GetDevice, request, false, route.extract(request->path));
    execCommand(cmd, response, headers);
}

void REST::UpdateDevice(const Route& route, Web::Request& request, Web::Response& response, Web::Headers& headers)
{
    auto cmd = createCommand(Operation::UpdateDevice, request, true, route.extract(request->path));
    execCommand(cmd, response, headers);
}

void REST::Rules(const Route& route, Web::Request& request, Web::Response& response, Web::Headers& headers)
{
    auto cmd = createCommand(Operation::GetRules, request, false);
    execCommand(cmd, response, headers);
}

void REST::GetRule(const Route& route, Web::Request& request, Web::Response& response, Web::Headers& headers)
{
    auto cmd = createCommand(Operation::GetRules, request, false, route.extract(request->path));
    execCommand(cmd, response, headers);
}

void REST::CreateRule(const Route& route, Web::Request& request, Web::Response& response, Web::Headers& headers)
{
    auto cmd = createCommand(request->method == "PUT"
                    ? Operation::CreateRule
                    : Operation::UpdateRule, request, true, route.extract(request->path));
    execCommand(cmd, response, headers);
}

void REST::DeleteRule(const Route& route, Web::Request& request, Web::Response& response, Web::Headers& headers)
{
    auto cmd = createCommand(Operation::DeleteRule, request, false, route.extract(request->path));
    execCommand(cmd, response, headers);
}

void REST::DeviceRules(const Route& route, Web::Request& request, Web::Response& response, Web::Headers& headers)
{
    auto cmd = createCommand(Operation::DeviceRules, request, false, route.extract(request->path));
    execCommand(cmd, response, headers);
}

void REST::SupportedRepeatTimeouts(const Route& route, Web::Request& request, Web::Response& response, Web::Headers& headers)
{
    auto cmd = createCommand(Operation::SupportedRepeatTimeouts, request, false);
    execCommand(cmd, response, headers);
}

void REST::SetValue(const Route& route, Web::Request& request, Web::Response& response, Web::Headers& headers)
{
    auto cmd = createCommand(Operation::Set, request, true, route.extract(request->path));
    execCommand(cmd, response, headers);
}

void REST::ScanWiFiNetworks(const Route& route, Web::Request& request, Web::Response& response, Web::Headers& headers)
{
    auto cmd = createCommand(Operation::ScanWiFiNetworks, request, false);
    execCommand(cmd, response, headers);
}

void REST::ListNetworks(const Route& route, Web::Request& request, Web::Response& response, Web::Headers& headers)
{
    auto cmd = createCommand(Operation::WiFiNetworks, request, false);
    execCommand(cmd, response, headers);
}

void REST::ConnectToWiFi(const Route& route, Web::Request& request, Web::Response& response, Web::Headers& headers)
{
    Command cmd = createCommand(Operation::ConnectToWiFi, request, true);
    execCommand(cmd, response, headers);
}
