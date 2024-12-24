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
#include "command_executor.h"
#include "qr.h"
#include <common/enumerations.h>
#include <common/http_helpers.h>
#include <common/log.h>
#include <common/singleton.h>
#include <micro_clients/network_manager.h>
#include <micro_clients/settings.h>
#include <nlohmann/json.hpp>
#include <random>
#include <stdio.h>
#include <types/serial.h>
#include <types/types.h>
#include <types/update_events.h>

using namespace aquabox::v1;
using namespace aquabox;

CommandRequest createCommand(Operation operation,
                      Web::Request request,
                      bool body,
                      std::unordered_map<std::string, std::string> params = std::unordered_map<std::string, std::string>())
{
    CommandRequest command;
    command.setOperation(operation);
    command.setParameters(params);
    if (body) {
        try {
            command.setData(nlohmann::json::parse(request->content.string()));
        }
        catch (...) {
            LOG_ERROR << "Can't set request data for operation " << (int) operation;
        }
    }
    return command;
}

void execCommand(CommandRequest& cmd, Web::Response response, Web::Headers headers)
{
    CommandExecutor exec;
    ExecutionResult result = exec.exec(cmd);
    if (result) {
        LOG_ERROR << "Command " << (int) (cmd.operation()) << " failed with: " << result.fail.message() << " details: " << result.fail.details();
    }
    else {
        LOG_DEBUG << "Command " << (int) (cmd.operation()) << " finished";
    }
    SimpleWeb::StatusCode code = SimpleWeb::StatusCode::server_error_not_implemented;
    switch (result.fail.result()) {
    case Result::BadCommand: {
        code = SimpleWeb::StatusCode::client_error_bad_request;
        break;
    }
    case Result::AccessDenied: {
        code = SimpleWeb::StatusCode::client_error_forbidden;
        break;
    }
    case Result::NotFound: {
        code = SimpleWeb::StatusCode::client_error_not_found;
        break;
    }
    case Result::ServerError: {
        code = SimpleWeb::StatusCode::server_error_internal_server_error;
        break;
    }
    case Result::Ok: {
        code = SimpleWeb::StatusCode::success_ok;
        break;
    }

    default:
        code = SimpleWeb::StatusCode::success_ok;
    }

    if (result.ok.data().is_null())
        response->write(code, headers);
    else
        response->write(code, as_json(result).dump(), headers);
}

nlohmann::json getStatusInfo(bool detailed)
{
    nlohmann::json host({{"rest", Singleton<ISettings>::i()->get(Settings::Keys::HTTP_PORT, Settings::Defaults::REST_PORT)},
                         {"stream", Singleton<ISettings>::i()->get(Settings::Keys::STREAM_PORT, Settings::Defaults::WS_PORT)},
                         {"serial", serial()}});

    if (detailed) {
        auto net = Singleton<INetworkManager>::i()->currentNetwork();
        host["network"] = nlohmann::json(
            {{"ssid", net.ssid}, {"ip", net.ip}, {"internet", Singleton<INetworkManager>::i()->hasInternetConnection()}});
    }

    host["host"] = boost::asio::ip::host_name();

    nlohmann::json status({{"aquabox", host}, {"status", "online"}});

    return status;
}

void REST::init(std::shared_ptr<HttpServer> http, Route base)
{
    API::init(http, base);

    m_statusQR = base.sub().word("status").word("qr");
    setup("PUT", m_statusQR, apiCallback<REST>(&REST::StatusQR));
}
/*
void REST::StaticFiles(const Route& route, Web::Request& request, Web::Response&
response, Headers& headers)
{
    Path p = Singleton<AquaBox>::get()->workingDirectory();
    p.append(request->path);
    p.normalize();

    if (!p.contains(Singleton<AquaBox>::get()->workingDirectory())) {
        response->write(SimpleWeb::StatusCode::client_error_forbidden, headers);
        return;
    }

    if (!p.exists()) {
        response->write(SimpleWeb::StatusCode::client_error_not_found, headers);
        return;
    }

    if (p.isDirectory()) {
        response->write(SimpleWeb::StatusCode::client_error_not_acceptable,
headers); return;
    }

    serveFile(response, p, headers, false);
}*/

void REST::StatusQR(const Route& route, Web::Request& request, Web::Response& response, Web::Headers& headers)
{
    std::string path = "./whoami_qr_" + std::to_string(std::rand()) + ".png";
    double factor = 0.;

    auto query = request->parse_query_string();

    if (query.count("factor") > 0) {
        std::string factorString = query.find("factor")->second;
        try {
            factor = std::stod(factorString);
        }
        catch (const std::invalid_argument&) {
            LOG_ERROR << "Bad zoom factor value \"" << factorString << "\"";
            factor = 0;
        }
    }

    if (factor < 0.01) {
        factor = 1;
    }

    LOG_DEBUG << "Generating QR with zoom factor " << factor;

    qr::Qr::create(getStatusInfo(false).dump(4), path, factor);

    HttpHelpers::serveFile(response, path, headers, true);
}

void REST::Status(const Route& route, Web::Request& request, Web::Response& response, Web::Headers& headers)
{
    auto cmd = createCommand(Operation::GetStatus, request, false);
    execCommand(cmd, response, headers);
}

// void REST::WiFiQR(const Route& route, Web::Request& request, Web::Response&
// response, Headers& headers)
// {
//     std::string path = "wifi_qr_" + std::to_string(std::rand()) + ".png";
//     double factor = 0.;

//     auto query = request->parse_query_string();
//     if (query.count("factor") > 0) {
//         std::string factorString = query.find("factor")->second;
//         try
//         {
//             factor = std::stod(factorString);
//         }
//         catch (const std::invalid_argument &)
//         {
//             LOG_ERROR << "Bad zoom factor value \"" << factorString << "\"";
//             factor = 0;
//         }
//     }

//     if (factor < 1.) {
//         factor = 1;
//     }

//     LOG_DEBUG << "Generating QR with zoom factor " << factor;
//     qr::Qr::create("asdasda", path, factor);

//     serveFile(response, path, headers, true);
// }

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
    auto cmd = createCommand(request->method == "PUT" ? Operation::CreateRule : Operation::UpdateRule,
                             request,
                             true,
                             route.extract(request->path));
    execCommand(cmd, response, headers);
}

void REST::DeleteRule(const Route& route, Web::Request& request, Web::Response& response, Web::Headers& headers)
{
    auto cmd = createCommand(Operation::DeleteRule, request, false, route.extract(request->path));
    execCommand(cmd, response, headers);
}

void REST::DeviceRules(const Route& route, Web::Request& request, Web::Response& response, Headers& headers)
{
    auto cmd = createCommand(Operation::DeviceRules, request, false, route.extract(request->path));
    execCommand(cmd, response, headers);
}

void REST::SetValue(const Route& route, Web::Request& request, Web::Response& response, Headers& headers)
{
    auto cmd = createCommand(Operation::Set, request, true, route.extract(request->path));
    execCommand(cmd, response, headers);
}

void REST::Call(const Route& route, Request& request, Response& response, Headers& headers)
{
    auto cmd = createCommand(Operation::Call, request, false);
    execCommand(cmd, response, headers);
}

void REST::ScanWiFiNetworks(const Route& route, Web::Request& request, Web::Response& response, Headers& headers)
{
    auto cmd = createCommand(Operation::ScanWiFiNetworks, request, false);
    execCommand(cmd, response, headers);
}

void REST::ListNetworks(const Route& route, Request& request, Response& response, Headers& headers)
{
    auto cmd = createCommand(Operation::WiFiNetworks, request, false);
    execCommand(cmd, response, headers);
}

void REST::ConnectToWiFi(const Route& route, Web::Request& request, Web::Response& response, Headers& headers)
{
    auto cmd = createCommand(Operation::ConnectToWiFi, request, true);
    execCommand(cmd, response, headers);
}
