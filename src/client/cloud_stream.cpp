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

#include "cloud_stream.h"
#include "cloud.h"
#include <common/log.h>
#include <common/singleton.h>

using namespace aquabox;

CloudStream::CloudStream(const std::string& url)
    : SimpleWeb::SocketClient<SimpleWeb::WS>(url)
    , m_url(url)
    , m_status(Status::Detached)
{
    LOG_DEBUG << "Creating stream for " << url;
    on_open = [this](std::shared_ptr<Connection> conenction) { onOpen(connection); };

    on_message = [this](std::shared_ptr<Connection> connection, std::shared_ptr<InMessage> message) { onMessage(connection, message); };

    on_close = [this](std::shared_ptr<CloudStream::Connection> connection, int status, const std::string& reason) {
        onClose(connection, status, reason);
    };

    on_error = [this](std::shared_ptr<CloudStream::Connection> connection, const SimpleWeb::error_code& code) { onError(connection, code); };

    m_thread = std::move(std::thread([this] {
        bool canStart = true;
        while (true) {
            if (m_reattach.compare_exchange_weak(canStart, false)) {
                SimpleWeb::SocketClient<SimpleWeb::WS>::start();
            }

            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }));
}

void CloudStream::attach()
{
    m_status = Status::Attaching;
    m_reattach.exchange(true);
}

CloudStream::~CloudStream()
{
    m_thread.join();
}

void CloudStream::onOpen(std::shared_ptr<CloudStream::Connection> connection)
{
    m_connection = connection;
    LOG_DEBUG << "Attached to stream " << m_url;
    m_status = Status::Attached;
    Singleton<Cloud>::i()->isRegistered = true;
}

void CloudStream::onMessage(std::shared_ptr<CloudStream::Connection> connection, std::shared_ptr<CloudStream::InMessage> message)
{
    m_status = Status::Attached;
    std::string messageString = "";
    ;
    try {
        messageString = message->string();
        const nlohmann::json json = nlohmann::json::parse(messageString);
        auto reply = messageReceived(json);
        connection->send(reply.dump(4));
    }
    catch (const std::exception& ex) {
        LOG_ERROR << "Invalid message from cloud \"" << messageString << "\" :" << ex.what();
    }
    catch (...) {
        LOG_ERROR << "Unknown error in message from cloud";
    }
}

void CloudStream::onClose(std::shared_ptr<CloudStream::Connection> connection, int status, const std::string& reason)
{
    LOG_INFO << "Closed connection to " << m_url << " because: " << reason << "(" << status << ")";
    m_status = Status::Detached;
    m_connection = nullptr;
    SimpleWeb::SocketClient<SimpleWeb::WS>::stop();
}

void CloudStream::onError(std::shared_ptr<CloudStream::Connection>, const SimpleWeb::error_code& code)
{
    LOG_INFO << "Error occured in connection to " << m_url << " error code " << code.message();
    m_status = Status::Detached;
    m_connection = nullptr;
    SimpleWeb::SocketClient<SimpleWeb::WS>::stop();
    Singleton<Cloud>::i()->isRegistered = false;
}

void CloudStream::sendMessage(const nlohmann::json& message)
{
    try
    {
        send(message.dump(4));
    }
    catch(...)
    {
        LOG_ERROR << "Can't sent message to stream " << m_url;
        m_status = Status::Detached;
    }
}

void CloudStream::send(const std::string data)
{
    if (m_connection) {
        m_connection->send(data);
    }
}
