#include "cloud.h"
#include "log.h"
#include "aquabox.h"
#include "serial.h"
#include "settings.h"
#include "commands_stream.h"
#include <nlohmann/json.hpp>


#define SERIAL_NUMBER "dfb1e54e863a175d0832615844d1e9bb08d8a9cd"

#define AUTH_URL "/hw/" SERIAL_NUMBER "/auth"

using namespace aquabox;

void Cloud::connect()
{
    for(auto s : m_streams)
    {
        try
        {
            if (!s.second->isAttached()) {
                s.second->attach();
            }
        }
        catch (std::exception& e) {
            LOG_ERROR << "Can't connect to stream " << s.first << ": " << e.what();
        } catch(...) {
            LOG_ERROR << "Can't connect to stream " << s.first;
        }
    }
}

void Cloud::disconnect()
{
}

bool Cloud::isRegistered()
{
    const std::string cloudBaseUrl = Settings::string(Settings::CLOUD_URL, "edwardoid.redirectme.net");
    const std::string cloudHttpUrl = cloudBaseUrl + ":4444";
    const std::string cloudWSUrl =  std::string("") + cloudBaseUrl + ":4444";
    LOG_INFO << "Checking if device is registered on " << cloudHttpUrl;
    HttpClient client(cloudHttpUrl);

    nlohmann::json authData(
        {
            { "type", "box" },
            { "serial", "some-generated-uuid" }
        }
    );

    try {
        auto response = client.request("POST", AUTH_URL, authData.dump(4));
        if (response->status_code == "200 OK") {
            nlohmann::json resp = nlohmann::json::parse(response->content);
            auto streamsAvailable = resp.find("streams");

            if (streamsAvailable->is_array()) {
                for (size_t i = 0; i < streamsAvailable.value().size(); ++i) {
                    auto stream = streamsAvailable.value().at(i);
                    if (stream.is_string()) {
                        auto name  = stream.get<std::string>();
                        std::string url = cloudWSUrl;
                        url += "/stream/";
                        url += SERIAL_NUMBER;
                        url += "/";
                        url += name;
                        bool insert = true;
                        if (m_streams.find(name) != m_streams.end()) {
                            insert = m_streams[name]->isAttached();
                        }

                        std::shared_ptr<CloudStream> stream;
                        
                        if (insert) {
                            if (name == "commands") {
                                stream = std::make_shared<CommandsStream>(url);
                            } else {
                                stream = std::make_shared<CloudStream>(url);
                            }
                            m_streams.insert({ name, stream});
                        }

                        LOG_INFO << "Stream " << name << " is available" << (m_streams[name]->isAttached() ? " and is already attached.": ".");
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
        LOG_ERROR << e.what() << " was connected to " << m_streams.size() << " streams";
        for (auto i : m_streams) {
            LOG_ERROR << "Trying to detach from " << i.first;
            i.second->detach();
        }
        m_streams.clear();
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