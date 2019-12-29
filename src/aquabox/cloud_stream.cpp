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

#include "cloud_stream.h"
#include "log.h"

using namespace aquabox;


CloudStream::CloudStream(std::string url)
    : SimpleWeb::SocketClient<SimpleWeb::WS>(url)
    , m_url(url)
    , m_attached(false)
{
    on_open = [this](std::shared_ptr<Connection> conenction) { onOpen(connection); };

    on_message = [this](std::shared_ptr<Connection> connection, std::shared_ptr<InMessage> message) { onMessage(connection, message); };

    on_close = [this](std::shared_ptr<CloudStream::Connection> connection, int status, const std::string & reason) { onClose(connection, status, reason); };

    on_error = [this](std::shared_ptr<CloudStream::Connection> connection, const SimpleWeb::error_code & code) { onError(connection, code); };
}

void CloudStream::attach()
{
    m_thread = std::thread([this]{
        LOG_DEBUG << "Attaching to " << m_url;
        SimpleWeb::SocketClient<SimpleWeb::WS>::start();
    });
}

bool CloudStream::isAttached() const
{
    return m_attached;
}

void CloudStream::detach()
{
    LOG_DEBUG << "Dettaching from " << m_url;
    //SimpleWeb::SocketClient<SimpleWeb::WS>::stop();
}

CloudStream::~CloudStream()
{
    detach();
}

void CloudStream::onOpen(std::shared_ptr<CloudStream::Connection> connection)
{
    m_connection = connection;
    m_attached = true;
}

void CloudStream::onMessage(std::shared_ptr<CloudStream::Connection> connection, std::shared_ptr<CloudStream::InMessage> message)
{
    m_attached = true;    
}

void CloudStream::onClose(std::shared_ptr<CloudStream::Connection> connection, int status, const std::string &reason)
{
    LOG_INFO << "Closed connection to " << m_url << " because: " << reason << "(" << status << ")";
    if (m_attached) {
        detach();
    }
    m_attached = false;
    m_connection = nullptr;
}

void CloudStream::onError(std::shared_ptr<CloudStream::Connection>, const SimpleWeb::error_code &code)
{
    LOG_INFO << "Error occured in connection to " << m_url << " error code " << code.message();
    if (m_attached) {
        detach();
    }
    m_attached = false;
    m_connection = nullptr;
}

void CloudStream::send(std::string data)
{
    if (m_connection) {
        m_connection->send(data);
    }
}
