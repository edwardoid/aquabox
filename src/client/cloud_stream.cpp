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
#include <types/serial.h>
#include <types/event.h>
#include <types/command.h>
#include <types/error_response.h>
#include "command_executor.h"
#include <common/log.h>
#include <common/singleton.h>
#include <micro_clients/settings.h>

using namespace aquabox;

std::string CloudStream::url()
{
    std::stringstream ss;
    ss  << Singleton<ISettings>::i()->get(Settings::Keys::CLOUD_URL, Settings::Defaults::CLOUD_URL)
        << ":"
        << Singleton<ISettings>::i()->get(Settings::Keys::CLOUD_PORT, Settings::Defaults::CLOUD_WS_PORT)
        << "/hw/api/v1/stream/"
        << SERIAL_NUMBER
        << "/commands";

    return ss.str();
}

CloudStream::CloudStream()
    : SimpleWeb::SocketClient<SimpleWeb::WS>(url())
    , m_status(Status::Detached)
{
    LOG_DEBUG << "Creating stream for " << url();
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

void CloudStream::checkIfRegistered()
{
    if (m_status < Status::Attaching)
    {
        attach();
    }
}

bool CloudStream::isRegistered()
{
    return m_status == Status::Attached;
}

void CloudStream::postUpdate(std::shared_ptr<Event> event)
{
    if (m_status == Status::Attached)
    {
        nlohmann::json json;
        json = *event;
        sendMessage(json);
    }
    else 
    {
        LOG_WARNING << "Cloud stream is not attached yet";
    }
    
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
    LOG_DEBUG << "Attached to stream";
    m_status = Status::Attached;
}

void CloudStream::onMessage(std::shared_ptr<CloudStream::Connection> connection, std::shared_ptr<CloudStream::InMessage> message)
{
    m_status = Status::Attached;
    std::string messageString = "";
    ;
    try {
        messageString = message->string();
        const nlohmann::json json = nlohmann::json::parse(messageString);
        messageReceived(json);
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
    LOG_INFO << "Closed connection to cloud because: " << reason << "(" << status << ")";
    m_status = Status::Detached;
    m_connection = nullptr;
    SimpleWeb::SocketClient<SimpleWeb::WS>::stop();
}

void CloudStream::onError(std::shared_ptr<CloudStream::Connection>, const SimpleWeb::error_code& code)
{
    LOG_INFO << "Error occured in connection to cloud error code " << code.message();
    m_status = Status::Detached;
    m_connection = nullptr;
    SimpleWeb::SocketClient<SimpleWeb::WS>::stop();
}

void CloudStream::sendMessage(const nlohmann::json& message)
{
    try
    {
        send(message.dump(4));
    }
    catch(...)
    {
        LOG_ERROR << "Can't sent message to the cloud stream";
        m_status = Status::Detached;
    }
}

void CloudStream::send(const std::string data)
{
    if (m_connection) {
        m_connection->send(data);
    }
}

void CloudStream::messageReceived(const nlohmann::json& message)
{
    sendMessage(exec(message));
}

nlohmann::json CloudStream::exec(const nlohmann::json& message)
{
    try {
        LOG_DEBUG << "Got command: " << message.dump(4);
        CommandRequest cmd;
        from_json(message, cmd);

        CommandExecutor exe;
        return as_json(exe.exec(cmd));
    }
    catch (nlohmann::json::parse_error err) {
        return as_json(ErrorResponse::parseError());;
    }
    catch (std::exception err) {
        ErrorResponse resp;
        resp.setMessage("Execution failed");
        resp.setResult(Result::ServerError);
        resp.setDetails(err.what());
        return as_json(resp);
    }
    catch (...) {
        return as_json(ErrorResponse::unknownError());;
    }
}
