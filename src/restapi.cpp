#include "restapi.h"
#include "log.h"
#include "qr.h"
#include "settings.h"
#include <stdio.h>
#include <random>
using namespace aquabox::rest;
using namespace Pistache;

DEFINE_API_IMPL(Status)
{
    Json res;
    res.set("setupComplete", false);
    res.set("ssid", Settings::string(Settings::SSID, ""));
    res.set("password", Settings::string(Settings::WIFI_PASSWORD, ""));
    res.set("encryption", Settings::string(Settings::WIFI_ENCRYPTION, "none"));

    response.send(Http::Code::Ok, res.str());
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