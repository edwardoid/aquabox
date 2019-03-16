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

DEFINE_API_IMPL(Status)
{
    nlohmann::json wifi({
        { "ssid" , Settings::string(Settings::SSID, "") },
        { "password" , Settings::string(Settings::WIFI_PASSWORD, "")},
        { "encryption" , Settings::string(Settings::WIFI_ENCRYPTION, "")}
    });

    nlohmann::json version({
        { "version" , Config::VERSION },
        { "build" , Config::GIT_HASH },
        { "major" , Config::VERSION_MAJOR },
        { "minor" , Config::VERSION_MINOR }
    });

    nlohmann::json status({
        { "setupComplete" , false },
        { "version" , version },
        { "wifi" , wifi }
    });

    response.send(Http::Code::Ok, status.dump(4));
}

DEFINE_API_IMPL(WiFiQR)
{
    std::string path = "wifi_qr_" + std::to_string(std::rand()) + ".png";
    double factor = 0.;
    
    if (request.query().has("factor"))
    {
        std::string factorString = request.query().get("factor").getOrElse("1.0");
        try {
            factor = std::stod(factorString);
        }
        catch(const std::invalid_argument&) {
            LOG_ERROR << "Bad zoom factor value \"" << factorString << "\"";
            factor = 0;
        }
    } 

    if (factor < 1.)
        factor = 1;

    LOG_DEBUG << "Generating QR with zoom factor " << factor;
    qr::Qr::create("asdasda", path, factor);

    Pistache::Http::serveFile(response, path).then([this, path](ssize_t bytes)
        {
            std::remove(path.c_str());
        },
        Async::IgnoreException);
}


DEFINE_API_IMPL(Devices)
{
    std::vector<nlohmann::json> all;
    for (auto& dev : AquaBox::i().devices())
    {
        all.push_back(dev.data());
    }
    nlohmann::json res = nlohmann::json::object();
    res["devices"] = all;
    response.send(Http::Code::Ok, res.dump(4));
}