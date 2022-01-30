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
#include "aquabox.h"
#include "commands_stream.h"
#include <types/cloud_event.h>
#include "events_stream.h"
#include <types/serial.h>
#include <types/event.h>
#include <common/log.h>
#include <common/singleton.h>
#include <nlohmann/json.hpp>
#include <micro_clients/settings.h>
#include <micro_clients/network_manager.h>

#define AUTH_URL "/hw/api/v1/register"

using namespace aquabox;

Cloud::Cloud()
    : m_updateTimeout(std::chrono::minutes(1))
    , isRegistered(false)
{
    isRegistered.connect([](bool registered) {
        Singleton<aquabox::AquaBox>::get()->postUpdate(
            std::make_shared<CloudEvent>(registered));
    });
}

void Cloud::connect()
{
    if (m_updateTimeout.expired() == 0) {
        return;
    }
    m_updateTimeout.reset();
    m_streams.for_each(
        [](const std::string& name, std::shared_ptr<CloudStream>& stream) {
            try {
                if (!stream->isAttached()) {
                    stream->attach();
                }
            } catch (std::exception& e) {
                LOG_ERROR << "Can't connect to stream " << name << ": " << e.what();
            } catch (...) {
                LOG_ERROR << "Can't connect to stream " << name;
            }
        });
}

void Cloud::disconnect() { }

bool Cloud::checkIfRegistered()
{
    if (m_updateTimeout.expired() == 0) {
        return isRegistered;
    }

    m_updateTimeout.reset();
    isRegistered = requestStreams();

    return isRegistered;
}

bool Cloud::requestStreams()
{
    bool streamsAreOk = true;

    m_streams.for_each([&streamsAreOk](auto& name, auto& stream) {
        streamsAreOk &= stream->isAttached();
    });

    if (streamsAreOk && !m_streams.empty()) {
        return streamsAreOk;
    }
    const auto cloudBaseUrl = Singleton<ISettings>::i()->get(Settings::Keys::CLOUD_URL, Settings::Defaults::CLOUD_URL);
    const std::string cloudHttpUrl = cloudBaseUrl + ":" + std::to_string(Singleton<ISettings>::i()->get(Settings::Keys::CLOUD_PORT, Settings::Defaults::CLOUD_PORT));
    const std::string cloudWSUrl = cloudBaseUrl + ":" + std::to_string(Singleton<ISettings>::i()->get(Settings::Keys::CLOUD_WS_PORT, Settings::Defaults::CLOUD_WS_PORT));
    LOG_INFO << "Checking if device is registered on " << cloudHttpUrl;
    HttpClient client(cloudHttpUrl);

    static const nlohmann::json authData(
        { { "type", "box" }, { "id", SERIAL_NUMBER }, { "version", "1.0" } });

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
                        auto name = stream.get<std::string>();
                        std::string url = cloudWSUrl;
                        url += "/hw/api/v1/stream/";
                        url += SERIAL_NUMBER;
                        url += "/";
                        url += name;
                        std::shared_ptr<CloudStream> existingStream = m_streams.get(name, nullptr);

                        if (existingStream) {
                            if (!existingStream->isAttached()) {
                                LOG_DEBUG << "Stream " << name
                                          << " exists but not attached. Reattaching...";
                                existingStream->attach();
                            }
                        } else {
                            std::shared_ptr<CloudStream> stream;
                            LOG_DEBUG << "Creating stream" << name;
                            if (name == "commands") {
                                stream = std::make_shared<CommandsStream>(url);
                            } else if (name == "events") {
                                stream = std::make_shared<EventsStream>(url);
                            }

                            if (stream)
                                m_streams.insert(name, stream);
                        }
                    }
                }
            }
            return true;
        } else {
            disconnect();
            LOG_ERROR << "Device is not registered. " << response->status_code
                      << " cause: " << response->content.string();
            return false;
        }
    } catch (std::exception& e) {
        LOG_ERROR << e.what() << " was connected to " << m_streams.size()
                  << " streams";
        return false;
    }
}

void Cloud::doRegister() { checkIfRegistered(); }

bool Cloud::isConnected() { return false; }

void Cloud::heartheat()
{
    static bool working = false;
    if (working) {
        return;
    }
    m_working = true;
    Singleton<AquaBox>::get()->doLater("cloud_connection_manager", [this]() {
        do {
            if (isConnected()) {
                break;
            }
            auto nm = Singleton<INetworkManager>::i();

            if (Singleton<INetworkManager>::i()->hasInternetConnection()) {
                break;
            }

            if (isRegistered) {
                connect();
            } else {
                doRegister();
            }
        } while (false);

        m_working = false;
    });
}

void Cloud::postUpdate(std::shared_ptr<Event> event)
{
    auto str = stream("events");
    if (str) {
        str->sendMessage(event->serialize());
    } else {
        LOG_WARNING << "Can't post update becase stream is not avaialable";
    }
}

std::shared_ptr<CloudStream> Cloud::stream(std::string name)
{
    return m_streams.get(name, nullptr);
}
