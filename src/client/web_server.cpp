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

#include "web_server.h"
#include <micro_clients/settings.h>

using namespace aquabox;

bool WebServer::start()
{
    const Route base = Route().word("api").word("v1");

    m_rest = std::thread([this, base]() {
        m_http = std::make_shared<Web::HttpServer>();
        m_http->config.port = Singleton<ISettings>::i()->get(Settings::Keys::HTTP_PORT, Settings::Defaults::REST_PORT);
        LOG_INFO << "Starting web server on " << m_http->config.address << ":" << m_http->config.port;
        RESTApi.init(m_http, base);

        m_http->start();
    });

    m_stream = std::thread([this, base]() {
        m_ws.config.port = Singleton<ISettings>::i()->get(Settings::Keys::STREAM_PORT, Settings::Defaults::WS_PORT);
        LOG_INFO << "Starting websocket server " << m_ws.config.address << ":" << m_ws.config.port;
        m_updates = base.sub().string(":boxId").string(":appId").word("updates");
        m_rpc = base.sub().string(":boxId").string(":appId").word("rpc");

        WS.Commands(m_ws, m_rpc);

        m_ws.start();
    });

    return true;
}

void WebServer::postUpdate(std::shared_ptr<Event> event)
{
    WS.postEvent(event);
}

WebServer::~WebServer()
{
    m_rest.join();
    m_stream.join();
}