#include "restapi.h"
#include "log.h"
#include "qr.h"
#include "settings.h"
#include "config.h"
#include "aquabox.h"
#include <nlohmann/json.hpp>
#include <stdio.h>
#include <random>
using namespace aquabox::rest;
using namespace Pistache;
using namespace aquabox;

nlohmann::json getStatusInfo() {
    nlohmann::json wifi({{"ssid", Settings::string(Settings::SSID, "")},
                         {"password", Settings::string(Settings::WIFI_PASSWORD, "")},
                         {"encryption", Settings::string(Settings::WIFI_ENCRYPTION, "")}});

    nlohmann::json version({{"version", Config::VERSION},
                            {"build", Config::GIT_HASH},
                            {"major", Config::VERSION_MAJOR},
                            {"minor", Config::VERSION_MINOR}});

    nlohmann::json host({
        {"port", Settings::integer(Settings::HTTP_PORT, Config::REST_PORT)}
    });

    char name[HOST_NAME_MAX];
    if (0 == gethostname(name, HOST_NAME_MAX))
        host["host"] = name;

    nlohmann::json status({{"aquabox", host},
                           {"setupComplete", false},
                           {"version", version},
                           {"wifi", wifi}});


    return status;
}

DEFINE_API_IMPL(WhoAmI)
{
    std::string path = "whoami_qr_" + std::to_string(std::rand()) + ".png";
    double factor = 0.;

    if (request.query().has("factor"))
    {
        std::string factorString = request.query().get("factor").getOrElse("1.0");
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

    Pistache::Http::serveFile(response, path).then([this, path](ssize_t bytes) {
        std::remove(path.c_str());
    }, Async::IgnoreException);
}

DEFINE_API_IMPL(Login)
{
    auto raw = request.body();
    for (auto user : AquaBox::i().users())
    {
        LOG_DEBUG << "Existing user " << user.second->token() << " comparing with " << raw;
        if (user.second->token() == raw)
        {
            response.send(Http::Code::Ok);
        }
    }

    response.send(Http::Code::Forbidden);
}

DEFINE_API_IMPL(Status)
{
    response.send(Http::Code::Ok, getStatusInfo().dump(4));
}

DEFINE_API_IMPL(WiFiQR)
{
    std::string path = "wifi_qr_" + std::to_string(std::rand()) + ".png";
    double factor = 0.;

    if (request.query().has("factor"))
    {
        std::string factorString = request.query().get("factor").getOrElse("1.0");
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

    Pistache::Http::serveFile(response, path).then([this, path](ssize_t bytes) {
        std::remove(path.c_str());
    },Async::IgnoreException);
}

DEFINE_API_IMPL(Devices)
{
    std::vector<nlohmann::json> all;
    for (auto &dev : AquaBox::i().devices())
    {
        all.push_back(dev.second->data());
    }
    nlohmann::json res = nlohmann::json::object();
    res["devices"] = all;
    response.send(Http::Code::Ok, res.dump(4));
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
    response.send(Http::Code::Ok, res.dump(4));
}

DEFINE_API_IMPL(DeviceRules)
{
    LOG_ERROR_IF(!request.hasParam(":dev")) << "No device parameter in URL";
    std::string dev = request.param(":dev").as<std::string>();
    std::vector<nlohmann::json> all;
    for (auto &rule : AquaBox::i().rules()) {
        if (((std::string) rule.second->Device) == dev) {
            nlohmann::json data;
            rule.second->json(data);
            all.push_back(data);
        }
    }
    nlohmann::json res({
        { "rules" , all },
        { "device" , dev }
    });
    response.send(Http::Code::Ok, res.dump(4));
}