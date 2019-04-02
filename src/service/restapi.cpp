#include "restapi.h"
#include "log.h"
#include "qr.h"
#include "settings.h"
#include "config.h"
#include "aquabox.h"
#include "repeattimeout.h"
#include "enumerations.h"
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
        nlohmann::json json;
        dev.second->json(json);
        all.push_back(json);
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

DEFINE_API_IMPL(GetRule)
{
    LOG_ERROR_IF(!request.hasParam(":rule")) << "No rule id parameter in URL";
    std::string rule = request.param(":rule").as<std::string>();

    auto all = AquaBox::i().rules();
    auto pos = all.find(rule);

    if (pos == all.end())
        response.send(Http::Code::Not_Found);
    else
    {
        nlohmann::json data;
        pos->second->json(data);
        response.send(Http::Code::Ok, data.dump(4));
    }
}

DEFINE_API_IMPL(CreateRule)
{
    LOG_ERROR_IF(!request.hasParam(":dev")) << "No device parameter in URL";
    std::string dev = request.param(":dev").as<std::string>();
    nlohmann::json data;
    nlohmann::json ruleData;
    try{
        ruleData = nlohmann::json::parse(request.body());
    }
    catch(nlohmann::detail::parse_error& e)
    {
        data["data"] = request.body();
        data["error"] = e.what();
        response.send(Http::Code::Bad_Request, data.dump(4));
        return;
    }

    std::shared_ptr<Rule> r = std::make_shared<Rule>();
    if(!r->parse(ruleData)) {
        response.send(Http::Code::Bad_Request, data.dump(4));
        return;
    }

    if(r->getDevice().resolve() == nullptr) {
        data["data"] = request.body();
        data["error"] = "Can't find device";
        response.send(Http::Code::Not_Found, data.dump(4));
        return;
    }

    if(!AquaBox::i().addRule(r, request.method() == Http::Method::Post))
    {
        response.send(Http::Code::Bad_Request, data.dump(4));
        return;
    }

    data["rule"] = ruleData;
    response.send(Http::Code::Ok, data.dump(4));
}

DEFINE_API_IMPL(DeviceRules)
{
    LOG_ERROR_IF(!request.hasParam(":dev")) << "No device parameter in URL";
    std::string dev = request.param(":dev").as<std::string>();
    std::vector<nlohmann::json> all;
    for (auto &rule : AquaBox::i().rules()) {
        if (((std::string) rule.second->getDevice().Value) == dev) {
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

DEFINE_API_IMPL(SupportedRepeatTimeouts)
{
    std::vector<RepeatUnit> units;
    for(auto u : Serializable<RepeatUnit>::val2Str)
    {
        units.push_back(u.first);
    }
    nlohmann::json res;
    res["units"] = Serializable<RepeatUnit>::val2Str;
    response.send(Http::Code::Ok, res.dump(4));
}