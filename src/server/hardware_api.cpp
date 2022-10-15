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
#include "box.h"
#include "boxes_manager.h"
#include "database.h"
#include "telegram_bot.h"
#include <common/log.h>
#include <micro_clients/settings.h>
#include <singleton.h>

#include <nlohmann/json.hpp>
#include <thread>

using namespace aquabox;

HardwareApi HardwareApi::self;

bool HardwareApi::start()
{
    LOG_INFO << "Starting Aquabox devices connection manager...";

    self.setupRoutes();

    HardwareApi::self.m_ws.config.port = Settings::integer(Settings::Keys::CLOUD_WS_PORT, Settings::Defaults::CLOUD_WS_PORT);
    LOG_INFO << "Starting Websocket server on " << self.m_ws.config.port;
    self.m_ws.start();
    return true;
}

void HardwareApi::setupRoutes()
{
    const Route base = Route().word("hw").word("api").word("v1");
    m_url = base.sub().word("stream").string(":id").word("commands");

    m_ws.endpoint[m_url.url()].on_open = [this](std::shared_ptr<Web::WSConnection> connection) {
        const auto id = m_url.extract(connection->path)[":id"];
        if (acceptConnectionFromHW(id, connection)) {
            BoxesManager::get(id)->commands = connection;
        }
    };

    m_ws.endpoint[m_url.url()].on_close = [this](std::shared_ptr<Web::WSConnection> connection, int status, const std::string& reason) {
        if (status == STREAM_CONNECTION_REJECTED) {
            return;
        }
        LOG_INFO << "Connection from " << connection->remote_endpoint().address().to_string() << " to " << connection->path << " closed.";
        const auto id = m_url.extract(connection->path)[":id"];
        BoxesManager::get(id)->commands.reset();
        BoxesManager::eraseIfNeed(id);
    };

    m_ws.endpoint[m_url.url()].on_error = [this](std::shared_ptr<Web::WSConnection> connection, SimpleWeb::error_code) {
        LOG_INFO << "Connection from " << connection->remote_endpoint().address().to_string() << " to " << connection->path
                 << " closed due error.";
        const auto id = m_url.extract(connection->path)[":id"];
        BoxesManager::get(id)->commands.reset();
        BoxesManager::eraseIfNeed(id);
    };

    m_ws.endpoint[m_url.url()].on_message = [this](std::shared_ptr<Web::WSConnection> connection,
                                                        std::shared_ptr<Web::Message> in_message) {
        std::string data = in_message->string();
        LOG_DEBUG << "On " << connection->path << " got some data: " << data;

        auto id = m_url.extract(connection->path)[":id"];

        auto box = BoxesManager::get(id);

        if (box) {
            box->ready(data);
        }
    };
}

bool HardwareApi::acceptConnectionFromHW(const std::string& id, std::shared_ptr<Web::WSConnection> connection)
{
    LOG_INFO << "Got connection from " << connection->remote_endpoint().address().to_string() << " for " << id << " to "
             << connection->path;
    if (!Database::i().wasProduced(id)) {
        LOG_WARNING << "Rejecting connection from " << connection->remote_endpoint().address().to_string() << " to stream for " << id;
        connection->send_close(STREAM_CONNECTION_REJECTED);
        return false;
    }
    return (BoxesManager::get(id, true) != nullptr) ? true : false;
}
