#include "restapi.h"
#include "log.h"
#include "qr.h"
#include "settings.h"
#include "config.h"
#include "aquabox.h"
#include "rule.h"
#include "device.h"
#include "user.h"
#include "repeattimeout.h"
#include "enumerations.h"
#include "path.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdio.h>
#include <random>

using namespace aquabox::rest;
using namespace aquabox;

void serveStream(Response response, Path path, std::shared_ptr<std::ifstream> ifs, size_t length, bool removeWhenFinished)
{
    std::vector<char> buffer(131072); // Safe when server is running on one thread
    std::streamsize read_length;
    if ((read_length = ifs->read(buffer.data(), static_cast<std::streamsize>(buffer.size())).gcount()) > 0)
    {
        length -= read_length;
        response->write(&buffer[0], read_length);
        if (read_length == static_cast<std::streamsize>(buffer.size()))
        {
            response->send([response, ifs, path, length, removeWhenFinished](const SimpleWeb::error_code &ec) {
                if (!ec)
                    serveStream(response, path, ifs, length, removeWhenFinished);
                else
                    LOG_DEBUG << "Connection interrupted while sending file ";
            });
        }
    }

    if (length == 0 && removeWhenFinished)
    {
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
    if (ifs->bad())
    {
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

nlohmann::json getStatusInfo()
{
    nlohmann::json wifi({{"ssid", Settings::string(Settings::SSID, "")},
                         {"password", Settings::string(Settings::WIFI_PASSWORD, "")},
                         {"encryption", Settings::string(Settings::WIFI_ENCRYPTION, "")}});

    nlohmann::json version({{"version", Config::VERSION},
                            {"build", Config::GIT_HASH},
                            {"major", Config::VERSION_MAJOR},
                            {"minor", Config::VERSION_MINOR}});

    nlohmann::json host({{"port", Settings::integer(Settings::HTTP_PORT, Config::REST_PORT)}});

    char name[HOST_NAME_MAX];
    if (0 == gethostname(name, HOST_NAME_MAX))
        host["host"] = name;

    nlohmann::json status({{"aquabox", host},
                           {"setupComplete", false},
                           {"version", version},
                           {"wifi", wifi}});

    return status;
}

DEFINE_API_IMPL(OptionsOK)
{
    if (request->method == "OPTIONS") {
        headers.emplace("Access-Control-Allow-Methods", "POST, GET, PUT, PATCH, DELETE, OPTIONS");
        response->write(SimpleWeb::StatusCode::success_no_content, headers);
    }
    else {
        response->write(SimpleWeb::StatusCode::client_error_bad_request, headers);
    }
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

    if (query.count("factor") > 0)
    {
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

    if (factor < 1.)
        factor = 1;

    LOG_DEBUG << "Generating QR with zoom factor " << factor;

    qr::Qr::create(getStatusInfo().dump(4), path, factor);

    serveFile(response, path, headers, true);
}

DEFINE_API_IMPL(Login)
{
    auto raw = request->content.string();
    for (auto user : AquaBox::i().users())
    {
        LOG_DEBUG << "Existing user " << user.second->token() << " comparing with " << raw;
        if (user.second->token() == raw)
        {
            response->write(SimpleWeb::StatusCode::success_ok, headers);
            return;
        }
    }

    response->write(SimpleWeb::StatusCode::client_error_forbidden, headers);
}

DEFINE_API_IMPL(Status)
{
    response->write(SimpleWeb::StatusCode::success_ok, getStatusInfo().dump(4), headers);
}

DEFINE_API_IMPL(WiFiQR)
{
    std::string path = "wifi_qr_" + std::to_string(std::rand()) + ".png";
    double factor = 0.;

    auto query = request->parse_query_string();
    if (query.count("factor") > 0)
    {
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

    if (factor < 1.)
        factor = 1;

    LOG_DEBUG << "Generating QR with zoom factor " << factor;
    qr::Qr::create("asdasda", path, factor);

    serveFile(response, path, headers, true);
}

DEFINE_API_IMPL(Devices)
{
    std::vector<nlohmann::json> all;
    for (auto &dev : AquaBox::i().devices())
    {
        nlohmann::json json;
        dev.second->json(json);
        json["isOn"] = dev.second->turnedOn();
        all.push_back(json);
    }
    nlohmann::json res = nlohmann::json::object();
    res["devices"] = all;
    response->write(SimpleWeb::StatusCode::success_ok, res.dump(4), headers);
}

DEFINE_API_IMPL(GetDevice)
{
    auto params = route.extract(request->path);
    if (params.count(":dev") == 0) {
        LOG_ERROR << "No device id parameter in URL";
        response->write(SimpleWeb::StatusCode::client_error_bad_request, headers);
        return;
    }
    std::string dev = params[":dev"];
    
    auto& devices = AquaBox::i().devices();
    auto pos = devices.find(dev);
    if (pos == devices.end()) {
        LOG_ERROR << "No device " << dev << " not found";
        response->write(SimpleWeb::StatusCode::client_error_not_found, headers);
        return;
    }

    nlohmann::json data;
    pos->second->json(data);
    data["isOn"] = pos->second->turnedOn();

    response->write(SimpleWeb::StatusCode::success_ok, data.dump(4), headers);
}

DEFINE_API_IMPL(Rules)
{
    std::vector<nlohmann::json> all;
    for (auto &rule : AquaBox::i().rules())
    {
        nlohmann::json data;
        rule.second->json(data);
        all.push_back(data);
    }
    nlohmann::json res = nlohmann::json::object();
    res["rules"] = all;
    response->write(SimpleWeb::StatusCode::success_ok, res.dump(4), headers);
}

DEFINE_API_IMPL(GetRule)
{
    auto params = route.extract(request->path);
    if (params.count(":rule") == 0)
    {
        LOG_ERROR << "No rule id parameter in URL";
        response->write(SimpleWeb::StatusCode::client_error_bad_request, headers);
        return;
    }
    std::string rule = params[":rule"];

    auto all = AquaBox::i().rules();
    auto pos = all.find(rule);

    if (pos == all.end())
        response->write(SimpleWeb::StatusCode::client_error_not_found, headers);
    else
    {
        nlohmann::json data;
        pos->second->json(data);
        response->write(SimpleWeb::StatusCode::success_ok, data.dump(4), headers);
    }
}

DEFINE_API_IMPL(CreateRule)
{
    auto params = route.extract(request->path);
    if (params.count(":dev") == 0)
    {
        LOG_ERROR << "No device id parameter in URL";
        response->write(SimpleWeb::StatusCode::client_error_bad_request, headers);
        return;
    }
    std::string dev = params[":dev"];
    nlohmann::json data;
    nlohmann::json ruleData;
    try
    {
        ruleData = nlohmann::json::parse(request->content.string());
    }
    catch (nlohmann::detail::parse_error &e)
    {
        data["data"] = request->content.string();
        data["error"] = e.what();
        response->write(SimpleWeb::StatusCode::client_error_bad_request, data.dump(4), headers);
        return;
    }

    std::shared_ptr<Rule> r = std::make_shared<Rule>();
    if (!r->parse(ruleData))
    {
        response->write(SimpleWeb::StatusCode::client_error_bad_request, data.dump(4), headers);
        return;
    }

    if (r->getDevice().resolve() == nullptr)
    {
        data["data"] = request->content.string();
        data["error"] = "Can't find device";
        response->write(SimpleWeb::StatusCode::client_error_bad_request, data.dump(4), headers);
        return;
    }

    if (!AquaBox::i().addRule(r, request->method == "GET"))
    {
        response->write(SimpleWeb::StatusCode::client_error_bad_request, data.dump(4), headers);
        return;
    }

    data["rule"] = ruleData;
    response->write(SimpleWeb::StatusCode::success_created, data.dump(4), headers);
}

DEFINE_API_IMPL(DeleteRule)
{
    auto params = route.extract(request->path);
    if (params.count(":rule") == 0) {
        LOG_ERROR << "No rule id parameter in URL";
        response->write(SimpleWeb::StatusCode::client_error_bad_request, headers);
        return;
    }
    std::string rule = params[":rule"];

    auto& all = AquaBox::i().rules();
    auto pos = all.find(rule);

    if (pos == all.end()) {
        LOG_DEBUG << "Requested to delete unexisting rule " << rule;
        response->write(SimpleWeb::StatusCode::client_error_not_found, headers);
        return;
    }

    if (AquaBox::i().removeRule(pos->second)) {
        LOG_DEBUG << "Rule " << pos->first << " removed";
        response->write(SimpleWeb::StatusCode::success_no_content, headers);
    } else {
        LOG_ERROR << "Rule " << pos->first << " removal failed";
        response->write(SimpleWeb::StatusCode::server_error_internal_server_error, headers);
    }
}

DEFINE_API_IMPL(DeviceRules)
{
    auto params = route.extract(request->path);
    if (params.count(":dev") == 0)
    {
        LOG_ERROR << "No dev id parameter in URL";
        response->write(SimpleWeb::StatusCode::client_error_bad_request, headers);
        return;
    }
    std::string dev = params[":dev"];
    std::vector<nlohmann::json> all;
    for (auto &rule : AquaBox::i().rules())
    {
        if (((std::string)rule.second->getDevice().Value) == dev)
        {
            nlohmann::json data;
            rule.second->json(data);
            all.push_back(data);
        }
    }
    nlohmann::json res({{"rules", all},
                        {"device", dev}});
    response->write(SimpleWeb::StatusCode::success_ok, res.dump(4), headers);
}

DEFINE_API_IMPL(SupportedRepeatTimeouts)
{
    std::vector<RepeatUnit> units;
    for (auto u : Serializable<RepeatUnit>::val2Str)
    {
        units.push_back(u.first);
    }
    nlohmann::json res;
    res["units"] = Serializable<RepeatUnit>::val2Str;
    response->write(SimpleWeb::StatusCode::success_ok, res.dump(4), headers);
}

DEFINE_API_IMPL(TurnOn)
{
    auto params = route.extract(request->path);
    if (params.count(":dev") == 0)
    {
        LOG_ERROR << "No dev id parameter in URL";
        response->write(SimpleWeb::StatusCode::client_error_bad_request, headers);
        return;
    }
    std::string dev = params[":dev"];
    auto pos = AquaBox::i().devices().find(dev);
    if (pos == AquaBox::i().devices().end()) {
        response->write(SimpleWeb::StatusCode::client_error_not_found, headers);
    } else {
        pos->second->turnOn();
        response->write(SimpleWeb::StatusCode::success_ok, headers);
    }
}

DEFINE_API_IMPL(TurnOff)
{
    auto params = route.extract(request->path);
    if (params.count(":dev") == 0)
    {
        LOG_ERROR << "No dev id parameter in URL";
        response->write(SimpleWeb::StatusCode::client_error_bad_request, headers);
        return;
    }
    std::string dev = params[":dev"];
    auto pos = AquaBox::i().devices().find(dev);
    if (pos == AquaBox::i().devices().end()) {
        response->write(SimpleWeb::StatusCode::client_error_not_found, headers);
    } else {
        pos->second->turnOff();
        response->write(SimpleWeb::StatusCode::success_ok, headers);
    }
}