#include "cloud.h"
#include "log.h"
#include "aquabox.h"
#include "serial.h"
#include "settings.h"
#include "commands_stream.h"
#include <nlohmann/json.hpp>


#define SERIAL_NUMBER "dfb1e54e863a175d0832615844d1e9bb08d8a9cd"

#define AUTH_URL "/hw/api/v1/register"

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
    const auto cloudBaseUrl = Settings::string(Settings::Keys::CLOUD_URL, Settings::Defaults::CLOUD_URL);
    const std::string cloudHttpUrl = cloudBaseUrl + ":" + std::to_string(Settings::integer(Settings::Keys::CLOUD_PORT, Settings::Defaults::CLOUD_PORT));
    const std::string cloudWSUrl =  std::string("") + ":" + std::to_string(Settings::integer(Settings::Keys::CLOUD_WS_PORT, Settings::Defaults::CLOUD_WS_PORT));
    LOG_INFO << "Checking if device is registered on " << cloudHttpUrl;
    HttpClient client(cloudHttpUrl);

    nlohmann::json authData(
        {
            { "type", "box" },
            { "id", SERIAL_NUMBER }
        }
    );

    try {
        auto response = client.request("POST", AUTH_URL, authData.dump(4));
        auto lenPos = response->header.find("Content-Length");
        if (response->status_code == "202 Accepted" && lenPos != response->header.end()) {

            int len = std::stoi(lenPos->second);
            std::string data = response->content.string().substr(0, len);
            nlohmann::json resp = nlohmann::json::parse(data);
            auto streamsAvailable = resp.find("streams");

            if (streamsAvailable->is_array()) {
                for (size_t i = 0; i < streamsAvailable.value().size(); ++i) {
                    auto stream = streamsAvailable.value().at(i);
                    if (stream.is_string()) {
                        auto name  = stream.get<std::string>();
                        std::string url = cloudWSUrl;
                        url += "/hw/api/v1/stream/";
                        url += SERIAL_NUMBER;
                        url += "/";
                        url += name;
                        std::shared_ptr<CloudStream> existingStream = nullptr;
                        if (m_streams.find(name) != m_streams.end()) {
                            existingStream = m_streams[name];
                        }
                        
                        if (existingStream) {
                            LOG_DEBUG << "Stream " << name << " exists";
                            if (!existingStream->isAttached())
                            {
                                LOG_DEBUG << "Stream " << name << " exists but not attached. Reattaching...";
                                existingStream->attach();
                            }
                        }
                        else {
                            std::shared_ptr<CloudStream> stream;
                            LOG_DEBUG << "Creating stream" << name;
                            if (name == "commands") {
                                stream = std::make_shared<CommandsStream>(url);
                            } else {
                                stream = std::make_shared<CloudStream>(url);
                            }
                            m_streams.insert({ name, stream});
                        }
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

std::shared_ptr<CloudStream> Cloud::stream(std::string name)
{
    auto p = m_streams.find(name);
    if (p == m_streams.end() || !p->second->isAttached()) {
        return nullptr;
    }
    return p->second;
}
