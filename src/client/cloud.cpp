/*
 * AquaBox
 * Copyright (C) 2019-2022  Eduard Sargsyan
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
#include "aquabox.h"
#include "commands_stream.h"
#include "events_stream.h"
#include <common/log.h>
#include <common/singleton.h>
#include <micro_clients/network_manager.h>
#include <micro_clients/settings.h>
#include <nlohmann/json.hpp>
#include <types/cloud_event.h>
#include <types/event.h>
#include <types/serial.h>

#define AUTH_URL "/hw/api/v1/register"

using namespace aquabox;

Cloud::Cloud()
    : m_updateTimeout(std::chrono::minutes(1))
    , isRegistered(false)
{
    m_updateTimeout.forceExpire();
    isRegistered.connect([](bool registered) { Singleton<aquabox::AquaBox>::get()->scheduleUpdate(std::make_shared<CloudEvent>(registered)); });
}

void Cloud::connect()
{
    connectToStreams();
}

bool Cloud::checkIfRegistered()
{
    if (m_updateTimeout.expired() == 0) {
        return isRegistered;
    }

    m_updateTimeout.reset();
    heartheat(true);

    return isRegistered;
}

bool Cloud::connectToStreams()
{
    bool connectionRequested = false;
    auto names = requiredStreamNames();
    std::stringstream ss;
    ss << Singleton<ISettings>::i()->get(Settings::Keys::CLOUD_URL, Settings::Defaults::CLOUD_URL);
    ss << ":" << Singleton<ISettings>::i()->get(Settings::Keys::CLOUD_WS_PORT, Settings::Defaults::CLOUD_WS_PORT);
    ss << "/hw/api/v1/stream/" << SERIAL_NUMBER << "/";
    for(auto& name : names)
    {
        std::unique_lock<std::mutex> ul(m_mx);
        auto& stream = m_streams[name];
        if (!stream)
        {
            std::string url = ss.str() + name;
            LOG_INFO << "Creating stream " << name;
            if (name == "commands") {
                stream = std::make_shared<CommandsStream>(url);
            }
            else if (name == "events") {
                stream = std::make_shared<EventsStream>(url);
            }
            connectionRequested = true;
        }

        if (stream->status() < CloudStream::Status::Attaching) {
            LOG_INFO << "Attaching to stream " << name;
            stream->attach();
            connectionRequested = true;
        } else {
            LOG_DEBUG << "Stream " << name << " status: " << (int) stream->status();
        }
    }

    return connectionRequested;
}

std::vector<std::string> Cloud::requiredStreamNames()
{
    std::vector<std::string> names;
    const auto cloudBaseUrl = Singleton<ISettings>::i()->get(Settings::Keys::CLOUD_URL, Settings::Defaults::CLOUD_URL);
    const std::string cloudHttpUrl = cloudBaseUrl + ":"
                                     + std::to_string(
                                         Singleton<ISettings>::i()->get(Settings::Keys::CLOUD_PORT, Settings::Defaults::CLOUD_PORT));
    LOG_INFO << "Checking if device is registered on " << cloudHttpUrl;
    HttpClient client(cloudHttpUrl);

    static const nlohmann::json authData({{"type", "box"}, {"id", SERIAL_NUMBER}, {"version", "1.0"}});

    try {
        auto response = client.request("POST", AUTH_URL, authData.dump(4));
        auto lenPos = response->header.find("Content-Length");
        if (response->status_code == "202 Accepted" && lenPos != response->header.end()) {
            int len = std::stoi(lenPos->second);
            std::string data = response->content.string().substr(0, len);
            nlohmann::json resp = nlohmann::json::parse(data);
            auto streamsAvailable = resp.find("streams");

            for (size_t i = 0;
                 streamsAvailable->is_array() && i < streamsAvailable.value().size();
                 ++i) {
                auto stream = streamsAvailable.value().at(i);
                if (stream.is_string()) {
                    names.push_back(stream.get<std::string>());
                }
            }
        }
        else {
            LOG_ERROR << "Device is not registered. " << response->status_code << " cause: " << response->content.string();
        }
    }
    catch (std::exception& e) {
        LOG_ERROR << e.what() << " was connected to " << m_streams.size() << " streams";
    }
    return names;
}

void Cloud::heartheat(bool force)
{
    if (!force && Singleton<INetworkManager>::i()->hasInternetConnection()) {
        return;
    }

    connect();

    isRegistered = postUpdate(std::make_shared<CloudEvent>(true));
}

bool Cloud::postUpdate(std::shared_ptr<Event> event)
{
    auto str = stream("events");
    if (str != nullptr && str->status() == CloudStream::Status::Attached) {
        str->sendMessage(event->serialize());
        return true;
    }
    else {
        LOG_WARNING << "Can't post update becase stream is not attached";
    }
    return false;
}

std::shared_ptr<CloudStream> Cloud::stream(std::string name)
{
    return m_streams[name];
}
