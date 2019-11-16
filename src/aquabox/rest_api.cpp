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
#include "qr.h"
#include "settings.h"
#include "aquabox.h"
#include "rule.h"
#include "device.h"
#include "relay.h"
#include "sensor.h"
#include "user.h"
#include "repeat_timeout.h"
#include "enumerations.h"
#include "path.h"
#include "update_events.h"
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

    if (cmd.response().empty())
        response->write(code, headers);
    else
        response->write(code, cmd.response(), headers);
}

void serveStream(API::Response response, Path path, std::shared_ptr<std::ifstream> ifs, size_t length, bool removeWhenFinished)
{
    std::vector<char> buffer(131072); // Safe when server is running on one thread
    std::streamsize read_length;
    if ((read_length = ifs->read(buffer.data(), static_cast<std::streamsize>(buffer.size())).gcount()) > 0) {
        length -= read_length;
        response->write(&buffer[0], read_length);
        if (read_length == static_cast<std::streamsize>(buffer.size())) {
            response->send([response, ifs, path, length, removeWhenFinished](const SimpleWeb::error_code &ec) {
                if (!ec)
                    serveStream(response, path, ifs, length, removeWhenFinished);
                else
                    LOG_DEBUG << "Connection interrupted while sending file ";
            });
        }
    }

    if (length == 0 && removeWhenFinished) {
        std::remove(path.c_str());
    }
}

void serveFile(API::Response response, Path path, API::Headers& headers, bool removeWhenFinished)
{
    static const std::unordered_map<std::string, std::string> ext2ContentType({
        { "js", "application/javascript" },
        { "ogg", "application/ogg" },
        { "pdf", "application/pdf" },
        { "xhtml", "application/xhtml+xml" },
        { "json", "application/json" },
        { "zip", "application/zip" },
        { "gif", "image/gif" },
        { "jpeg", "image/jpeg" },
        { "jpg", "image/jpeg" },
        { "png", "image/png" },
        { "tif", "image/tiff" },
        { "tiff", "image/tiff" },
        { "ico", "image/x-icon"},
        { "djvu", "image/vnd.djvu"},
        { "svg", "image/svg+xml"},
        { "css", "text/css"},
        { "csv", "text/csv"},
        { "html", "text/html"},
        { "htm", "text/html"},
        { "txt", "text/plain"},
        { "xml", "text/xml"}
    });


    auto ifs = std::make_shared<std::ifstream>(path, std::ifstream::in | std::ios::binary | std::ios::ate);
    if (ifs->bad()) {
        LOG_DEBUG << "File '" << path << "'can't be found/opened so send over http";
        response->write(SimpleWeb::StatusCode::client_error_not_found, headers);
        return;
    }

    auto length = ifs->tellg();
    ifs->seekg(0, std::ios::beg);

    auto mime = ext2ContentType.find(path.extension());

    if (mime == ext2ContentType.end()) {
        headers.find("Content-Type")->second = "application/octet-stream";
    } else {
        headers.find("Content-Type")->second = mime->second;
    }

    headers.emplace("Content-Length", std::to_string(length));
    response->write(headers);
    serveStream(response, path, ifs, length, removeWhenFinished);
}

nlohmann::json getStatusInfo(bool detailed)
{
    nlohmann::json host({
        {"rest", Settings::integer(Settings::Keys::HTTP_PORT, Settings::Defaults::REST_PORT)},
        {"stream", Settings::integer(Settings::Keys::STREAM_PORT, Settings::Defaults::WS_PORT)}
    });

    if (detailed) {
        std::string wifiState = "unknown";
        switch(AquaBox::i().networking().state())
        {
            case IoT::WiFi::State::InAPMode:
            case IoT::WiFi::State::SwitchingToAP: {
                wifiState = "ap";
                break;
            }

            case IoT::WiFi::Disconnected: {
                wifiState = "disconnected";
                break;
            }

            case IoT::WiFi::Connected: {
                wifiState = "connected";
                break;
            }

            default: {}
        } 
        host["network"] = nlohmann::json({
            { "ssid", AquaBox::i().networking().currentSSID() },
            { "ip", AquaBox::i().networking().currentIP() },
            { "mode",  wifiState }});
    }

    host["host"] = boost::asio::ip::host_name();

    nlohmann::json status({{"aquabox", host}});

    return status;
}

void REST::init(std::shared_ptr<HttpServer> http, Route base)
{
    API::init(http, base);

    m_statusQR = base.sub().word("status").word("qr");
    setup("PUT", m_statusQR, apiCallback<REST>(&REST::StatusQR));
}
/*
void REST::StaticFiles(const Route& route, API::Request& request, API::Response& response, Headers& headers)
{
    Path p = AquaBox::i().workingDirectory();
    p.append(request->path);
    p.normalize();

    if (!p.contains(AquaBox::i().workingDirectory())) {
        response->write(SimpleWeb::StatusCode::client_error_forbidden, headers);
        return;
    }

    if (!p.exists()) {
        response->write(SimpleWeb::StatusCode::client_error_not_found, headers);
        return;
    }

    if (p.isDirectory()) {
        response->write(SimpleWeb::StatusCode::client_error_not_acceptable, headers);
        return;
    }

    serveFile(response, p, headers, false);
}*/

void REST::StatusQR(const Route& route, API::Request& request, API::Response& response, Headers& headers)
{
    std::string path = "./whoami_qr_" + std::to_string(std::rand()) + ".png";
    double factor = 0.;

    auto query = request->parse_query_string();

    if (query.count("factor") > 0) {
        std::string factorString = query.find("factor")->second;
        try
        {
            factor = std::stod(factorString);
        }
        catch (const std::invalid_argument &)
        {
            LOG_ERROR << "Bad zoom factor value \"" << factorString << "\"";
            factor = 0;
        }
    }

    if (factor < 0.01) {
        factor = 1;
    }

    LOG_DEBUG << "Generating QR with zoom factor " << factor;

    qr::Qr::create(getStatusInfo(false).dump(4), path, factor);

    serveFile(response, path, headers, true);
}

void REST::Status(const Route& route, API::Request& request, API::Response& response, Headers& headers)
{
    auto cmd = createCommand(Operation::GetStatus, request, true);
    execCommand(cmd, response, headers);
}

// void REST::WiFiQR(const Route& route, API::Request& request, API::Response& response, Headers& headers)
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