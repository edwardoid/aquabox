#include "cloud.h"
#include "log.h"
#include "aquabox.h"
#include <nlohmann/json.hpp>

using namespace aquabox;

#define CLOUD_WS std::string("ws://edwardoid.redirectme.net:5555")
#define CLOUD_HTTP std::string("edwardoid.redirectme.net:4444")

void Cloud::connect()
{
    for(auto s : m_streams)
    {
        s.second->connect();
    }
}

void Cloud::disconnect()
{
}

bool Cloud::isRegistered()
{
    LOG_INFO << "Checking if device is registered on " << CLOUD_HTTP;
    HttpClient client(CLOUD_HTTP);

    nlohmann::json authData(
        {
            { "type", "box" },
            { "serial", "some-generated-uuid" }
        }
    );

    try {
        auto response = client.request("POST", "/hw/auth", authData.dump(4));
        if (response->status_code == "200 OK") {
            nlohmann::json resp = nlohmann::json::parse(response->content);
            auto streamsAvailable = resp.find("streams");
            
            for(auto s : m_streams)
            {
                if(s.second != nullptr)
                    s.second->disconnect();
            }


            if (streamsAvailable->is_array()) {
                for (size_t i = 0; i < streamsAvailable.value().size(); ++i) {
                    auto stream = streamsAvailable.value().at(i);
                    if (stream.is_string()) {
                        auto name  = stream.get<std::string>();
                        LOG_INFO << "Stream " << name << " is available.";
                        m_streams.insert({ name, std::make_shared<CloudStream>(name) });
                    }
                }
            }
            LOG_ERROR << "Device is registered.";
            return true;
        } else {
            disconnect();
            LOG_ERROR << "Device is not registered. " << response->status_code;
            return false;
        }
    }
    catch(std::exception& e) {
        LOG_ERROR << e.what();
        return false;
    }
}

void Cloud::doRegister()
{

}

bool Cloud::isConnected()
{
    return false;
}

void Cloud::heartheat()
{
    static bool working = false;
    if (working) {
        return;
    }
    working = true;
    AquaBox::i().doLater([this, &working](){
        do {
            if (isConnected()) {
                break;
            }
            if (AquaBox::i().networking().state() != IoT::WiFi::State::Connected &&
                AquaBox::i().networking().state() != IoT::WiFi::State::InAPMode) {
                break;
            }

            if (isRegistered()) {
                connect();
            } else {
                doRegister();
            }
        }
        while(false);

        working = false;
    });
}