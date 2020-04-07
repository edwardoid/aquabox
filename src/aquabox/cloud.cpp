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

#include "cloud.h"
#include "log.h"
#include "aquabox.h"
#include "serial.h"
#include "settings.h"
#include "event.h"
#include "commands_stream.h"
#include "network_manager.h"
#include <nlohmann/json.hpp>

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
    const std::string cloudWSUrl =  cloudBaseUrl + ":" + std::to_string(Settings::integer(Settings::Keys::CLOUD_WS_PORT, Settings::Defaults::CLOUD_WS_PORT));
    LOG_INFO << "Checking if device is registered on " << cloudHttpUrl;
    HttpClient client(cloudHttpUrl);

    nlohmann::json authData(
        {
            { "type", "box" },
            { "id", SERIAL_NUMBER },
            { "version", "1.0"}
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
            LOG_ERROR << "Device is not registered. " << response->status_code << " cause: " << response->content.string();
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
    m_working = true;
    AquaBox::i().doLater([this](){
        do {
            if (isConnected()) {
                break;
            }
            if (NetworkManager::i().connectedToInternet()) {
                break;
            }

            if (isRegistered()) {
                connect();
            } else {
                doRegister();
            }
        }
        while(false);

        m_working = false;
    });
}

void Cloud::postUpdate(std::shared_ptr<Event> event)
{
    auto str = stream("events");
    if (str) {
        str->send(event->serialize().dump(4));
    } else {
        LOG_WARNING << "Can't post update becase stream is not avaialable";
    }
}

std::shared_ptr<CloudStream> Cloud::stream(std::string name)
{
    auto p = m_streams.find(name);
    if (p == m_streams.end() || !p->second->isAttached()) {
        return nullptr;
    }
    return p->second;
}
