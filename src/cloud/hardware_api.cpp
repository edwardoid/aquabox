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

#include "hardware_api.h"
#include "boxes_manager.h"
#include "box.h"
#include "database.h"
#include "log.h"
#include "settings.h"

#include <thread>
#include <nlohmann/json.hpp>

using namespace aquabox;

HardwareApi HardwareApi::self;

bool HardwareApi::start()
{
    LOG_INFO << "Starting Aquabox devices connection manager...";

    self.setupRoutes();

    static std::thread managerThread([](){
       HardwareApi::self.m_ws.config.port = Settings::integer(Settings::Keys::CLOUD_WS_PORT, Settings::Defaults::CLOUD_WS_PORT);
       LOG_INFO << "Starting Websocket server on " << self.m_ws.config.port;
       self.m_ws.start();
    });
    self.m_http.config.port = Settings::integer(Settings::Keys::CLOUD_PORT, Settings::Defaults::CLOUD_PORT);
    LOG_INFO << "Starting HTTP server on " << self.m_http.config.port;
    self.m_http.start();
    return true;
}

void HardwareApi::setupRoutes()
{
    const Route base = Route().word("hw").word("api").word("v1");
    m_register = base.sub().word("register");
    m_login = base.sub().word("login");


    m_http.resource[m_register.url()]["POST"] = [this](Web::Response response, Web::Request request) {
        try {
            auto registerData = nlohmann::json::parse(request->content.string());
            if (!registerData.contains("id")) {
                response->write(SimpleWeb::StatusCode::client_error_bad_request, nlohmann::json({ { "error", "Box id is missing"} }).dump(4));
                return;
            }
            auto id = registerData["id"].get<std::string>();
            if (Database::i().wasProduced(id)) {
                response->write(SimpleWeb::StatusCode::success_accepted, nlohmann::json({
                                                                                            { "streams", { "log", "events", "commands"}},
                                                                                            { "id", id }
                                                                                        }).dump(4),
                                {{ "Content-Type", "application/json" }});
            } else {
                response->write(SimpleWeb::StatusCode::client_error_method_not_allowed);
            }
        }
        catch (nlohmann::json::exception& err) {
            LOG_ERROR << "Box registrarion failed: " << err.what();
            response->write(SimpleWeb::StatusCode::client_error_bad_request, nlohmann::json({ { "error", err.what() } }).dump(4));
        }
        catch (std::exception& err) {
            LOG_ERROR << "Box registrarion failed: " << err.what();
            response->write(SimpleWeb::StatusCode::server_error_internal_server_error, nlohmann::json({ { "error", err.what() } }).dump(4));
        }
        catch (...) {
            LOG_ERROR << "Box registrarion failed with unknown issue";
            response->write(SimpleWeb::StatusCode::server_error_internal_server_error);
        }
    };

    m_http.resource[m_login.url()]["POST"] = [this](Web::Response response, Web::Request request) {

    };

    m_events = base.sub().word("stream").string(":id").word("events");
    m_commands = base.sub().word("stream").string(":id").word("commands");

    m_ws.endpoint[m_events.url()].on_open = [this](std::shared_ptr<Web::WSConnection> connection) {
        const auto id = m_events.extract(connection->path)[":id"];
        if(acceptConnectionFromHW(id, connection)) {
            BoxesManager::get(id)->events = connection; 
        }
    };

    m_ws.endpoint[m_events.url()].on_close = [this](std::shared_ptr<Web::WSConnection> connection,
                                                    int status, const std::string & reason) {
        if (status == STREAM_CONNECTION_REJECTED) {
            return;
        }
        LOG_INFO << "Connection from " << connection->remote_endpoint_address() << " to " << connection->path << " closed.";
        const auto id = m_events.extract(connection->path)[":id"];
        BoxesManager::get(id)->events.reset();
        BoxesManager::eraseIfNeed(id);
    };

    m_ws.endpoint[m_events.url()].on_error = [this](std::shared_ptr<Web::WSConnection> connection,
                                                 SimpleWeb::error_code) {
        LOG_INFO << "Connection from " << connection->remote_endpoint_address() << " to " << connection->path << " closed due error.";
        const auto id = m_events.extract(connection->path)[":id"];
        BoxesManager::get(id)->events.reset();
        BoxesManager::eraseIfNeed(id);
    };

    m_ws.endpoint[m_events.url()].on_message = [this](std::shared_ptr<Web::WSConnection> connection,
                                                      std::shared_ptr<Web::Message> in_message) {
        LOG_DEBUG << "On " << connection->path << " got:\n\t\t" << in_message->string();
    };

    m_ws.endpoint[m_commands.url()].on_open = [this](std::shared_ptr<Web::WSConnection> connection) {
        const auto id = m_commands.extract(connection->path)[":id"];
        if(acceptConnectionFromHW(id, connection)) {
            BoxesManager::get(id)->commands = connection; 
        }
    };

    m_ws.endpoint[m_commands.url()].on_close = [this](std::shared_ptr<Web::WSConnection> connection,
                                                      int status, const std::string & reason) {
        if (status == STREAM_CONNECTION_REJECTED) {
            return;
        }
        LOG_INFO << "Connection from " << connection->remote_endpoint_address() << " to " << connection->path << " closed.";
        const auto id = m_commands.extract(connection->path)[":id"];
        BoxesManager::get(id)->commands.reset();
        BoxesManager::eraseIfNeed(id);
    };

    m_ws.endpoint[m_commands.url()].on_error = [this](std::shared_ptr<Web::WSConnection> connection,
                                                      SimpleWeb::error_code) {
        LOG_INFO << "Connection from " << connection->remote_endpoint_address() << " to " << connection->path << " closed due error.";
        const auto id = m_commands.extract(connection->path)[":id"];
        BoxesManager::get(id)->commands.reset();
        BoxesManager::eraseIfNeed(id);
    };

    m_ws.endpoint[m_commands.url()].on_message = [this](std::shared_ptr<Web::WSConnection> connection,
                                                        std::shared_ptr<Web::Message> in_message) {

        LOG_DEBUG << "On " << connection->path << " got some data";
        
        auto id = m_commands.extract(connection->path)[":id"];

        auto box = BoxesManager::get(id);

        if (box) {
            box->ready(in_message->string());
        }
    };
}

bool HardwareApi::acceptConnectionFromHW(std::string id, std::shared_ptr<Web::WSConnection> connection)
{
    LOG_INFO << "Got connection from " << connection->remote_endpoint_address()
             << " for " << id << " to " << connection->path;
    if (!Database::i().wasProduced(id)) {
        LOG_WARNING << "Rejecting connection from " << connection->remote_endpoint_address() << " to stream for " << id;
        connection->send_close(STREAM_CONNECTION_REJECTED);
        return false;
    }
    return (BoxesManager::get(id, true) != nullptr) ? true : false;
}
