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

using namespace aquabox::rest;
using namespace aquabox;

Command createCommand(Operation operation, Request request, bool body, std::unordered_map<std::string, std::string> params = {})
{
    Command command;
    command.setOperation(operation);
    command.setParameters(params);
    if (body) {
        command.setBody(request->content.string());
    }
    return command;
}

void execCommand(Command& cmd, Response response, Headers headers)
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


void API::init(HttpServerImpl& server)
{
    static aquabox::Route route;
    server.default_resource["OPTIONS"] = std::bind(&API::OptionsOK, this, route, std::placeholders::_1, std::placeholders::_2);
    server.default_resource["GET"] = std::bind(&API::OptionsOK, this, route, std::placeholders::_1, std::placeholders::_2);
    server.default_resource["POST"] = std::bind(&API::OptionsOK, this, route, std::placeholders::_1, std::placeholders::_2);
    server.default_resource["PUT"] = std::bind(&API::OptionsOK, this, route, std::placeholders::_1, std::placeholders::_2);
    server.default_resource["DELETE"] = std::bind(&API::OptionsOK, this, route, std::placeholders::_1, std::placeholders::_2);
}

void API::setup(HttpServerImpl& server,
                std::string method,
                Route& route,
                void (API::* Callback)(const aquabox::Route&, aquabox::rest::Response, aquabox::rest::Request))
{
    server.resource[route.url()][method] = std::bind(Callback, this, route, std::placeholders::_1, std::placeholders::_2);
    server.resource[route.url()]["OPTIONS"] = std::bind(&API::OptionsOK, this, route, std::placeholders::_1, std::placeholders::_2);
}

void serveStream(Response response, Path path, std::shared_ptr<std::ifstream> ifs, size_t length, bool removeWhenFinished)
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

void serveFile(Response response, Path path, Headers& headers, bool removeWhenFinished)
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

DEFINE_API_IMPL(OptionsOK)
{
    if (headers.count(Access_Control_Allow_Origin) == 0)
        headers.emplace(Access_Control_Allow_Origin, "*");
    if (headers.count(Access_Control_Allow_Headers) == 0)
        headers.emplace(Access_Control_Allow_Headers, "Origin, X-Requested-With, Content-Type, Accept");
    if (headers.count(Access_Control_Allow_Methods) == 0)
        headers.emplace(Access_Control_Allow_Methods, "POST, GET, PUT, PATCH, DELETE, OPTIONS");
    response->write(SimpleWeb::StatusCode::success_ok, headers);
}

DEFINE_API_IMPL(StaticFiles)
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
}

DEFINE_API_IMPL(StatusQR)
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

DEFINE_API_IMPL(Login)
{
    auto raw = request->content.string();
    for (auto user : AquaBox::i().users()) {
        LOG_DEBUG << "Existing user " << user.second->token() << " comparing with " << raw;
        if (user.second->token() == raw) {
            response->write(SimpleWeb::StatusCode::success_ok, headers);
            return;
        }
    }

    response->write(SimpleWeb::StatusCode::client_error_forbidden, headers);
}

DEFINE_API_IMPL(Status)
{
    auto cmd = createCommand(Operation::GetStatus, request, true);
    execCommand(cmd, response, headers);
}

DEFINE_API_IMPL(WiFiQR)
{
    std::string path = "wifi_qr_" + std::to_string(std::rand()) + ".png";
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

    if (factor < 1.) {
        factor = 1;
    }

    LOG_DEBUG << "Generating QR with zoom factor " << factor;
    qr::Qr::create("asdasda", path, factor);

    serveFile(response, path, headers, true);
}

DEFINE_API_IMPL(Devices)
{
    auto cmd = createCommand(Operation::GetDevices, request, false);
    execCommand(cmd, response, headers);
}

DEFINE_API_IMPL(GetDevice)
{
    auto cmd = createCommand(Operation::GetDevice, request, false, route.extract(request->path));
    execCommand(cmd, response, headers);
}

DEFINE_API_IMPL(UpdateDevice)
{
    auto cmd = createCommand(Operation::UpdateDevice, request, true, route.extract(request->path));
    execCommand(cmd, response, headers);
}

DEFINE_API_IMPL(Rules)
{
    auto cmd = createCommand(Operation::GetRules, request, false);
    execCommand(cmd, response, headers);
}

DEFINE_API_IMPL(GetRule)
{
    auto cmd = createCommand(Operation::GetRules, request, false, route.extract(request->path));
    execCommand(cmd, response, headers);
}

DEFINE_API_IMPL(CreateRule)
{
    auto cmd = createCommand(request->method == "PUT"
                    ? Operation::CreateRule
                    : Operation::UpdateRule, request, true, route.extract(request->path));
    execCommand(cmd, response, headers);
}

DEFINE_API_IMPL(DeleteRule)
{
    auto cmd = createCommand(Operation::DeleteRule, request, false, route.extract(request->path));
    execCommand(cmd, response, headers);
}

DEFINE_API_IMPL(DeviceRules)
{
    auto cmd = createCommand(Operation::DeviceRules, request, false, route.extract(request->path));
    execCommand(cmd, response, headers);
}

DEFINE_API_IMPL(SupportedRepeatTimeouts)
{
    auto cmd = createCommand(Operation::SupportedRepeatTimeouts, request, false);
    execCommand(cmd, response, headers);
}

DEFINE_API_IMPL(TurnOn)
{
    auto cmd = createCommand(Operation::TurnOn, request, false, route.extract(request->path));
    execCommand(cmd, response, headers);
}

DEFINE_API_IMPL(TurnOff)
{
    auto cmd = createCommand(Operation::TurnOff, request, false, route.extract(request->path));
    execCommand(cmd, response, headers);
}

DEFINE_API_IMPL(ScanWiFiNetworks)
{
    auto cmd = createCommand(Operation::ScanWiFiNetworks, request, false);
    execCommand(cmd, response, headers);
}

DEFINE_API_IMPL(ConnectToWiFi)
{
    Command cmd = createCommand(Operation::ConnectToWiFi, request, true);
    execCommand(cmd, response, headers);
}