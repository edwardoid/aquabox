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

#include "applications_api.h"
#include "box.h"
#include "boxes_manager.h"
#include "database.h"
#include "route.h"
#include "telegram_bot.h"
#include "command_executor.h"
#include <micro_clients/settings.h>
#include <types/error_response.h>

using namespace aquabox;

static std::function<void(const SimpleWeb::error_code &)> Fail([](const SimpleWeb::error_code& e) {
    if (e) LOG_WARNING << "Write to ws filed with: " << e.message() << " (code: " << e.value() << ")";
});

void ApplicationsApi::start()
{
    m_base = Route().word("api").word("v1");

    m_httpRoutine = std::move(std::thread(std::bind(&ApplicationsApi::startWebServer, this)));

    m_streamRoutine = std::move(std::thread(std::bind(&ApplicationsApi::startStreamServer, this)));
}

void ApplicationsApi::startWebServer()
{
    m_http = std::make_shared<HttpServer>();
    m_http->config.port = Settings::integer(Settings::Keys::HTTP_PORT, Settings::Defaults::REST_PORT);
    LOG_INFO << "Starting web server on " << m_http->config.address << ":" << m_http->config.port;
    RESTApi.init(m_http, m_base);

    m_http->start();
}

void ApplicationsApi::startStreamServer()
{
    m_ws = std::make_shared<WSServer>();
    m_ws->config.port = Settings::integer(Settings::Keys::STREAM_PORT, Settings::Defaults::WS_PORT);
    LOG_INFO << "Starting websocket server " << m_ws->config.address << ":" << m_ws->config.port;

    m_rpc = m_base.sub().string(":boxId").string(":appId").word("rpc");

    // Got new connection from an application
    m_ws->endpoint[m_rpc.url()].on_open = std::bind(& ApplicationsApi::onOpen,  this,
                                                                                std::placeholders::_1);

    m_ws->endpoint[m_rpc.url()].on_close = std::bind(& ApplicationsApi::onClose,this,
                                                                                std::placeholders::_1,
                                                                                std::placeholders::_2,
                                                                                std::placeholders::_3);

    m_ws->endpoint[m_rpc.url()].on_error = std::bind(& ApplicationsApi::onError, this, std::placeholders::_1, std::placeholders::_2);

    m_ws->endpoint[m_rpc.url()].on_message = std::bind(& ApplicationsApi::onMessage, this, std::placeholders::_1, std::placeholders::_2);

    m_ws->start();
}

void ApplicationsApi::removeConnection(const std::string& boxId, std::shared_ptr<WSServer::Connection> connection)
{
    auto box = BoxesManager::get(boxId);
    if (box == nullptr) {
        LOG_ERROR << "Can't remove connection to box " << boxId << " because it is missed";
        return;
    }

    auto pos = std::find_if(m_connections.begin(), m_connections.end(), [connection](auto& c)
    {
        return c.second == connection;
    });
    if (pos != m_connections.end()) {
        m_connections.erase(pos);
        LOG_INFO << "Removing application connection " << pos->first;
    }
}

void ApplicationsApi::createConnection(std::shared_ptr<WSServer::Connection> ws, const std::string& boxId)
{
    static std::atomic_uint32_t id(0);
    auto box = BoxesManager::get(boxId, true);
    m_connections.insert({ id.fetch_add(1),  ws});
    LOG_INFO << "Got new application connection. Assigned " << id.load();
}

void ApplicationsApi::onOpen(std::shared_ptr<WSServer::Connection> connection)
{
        auto params = m_rpc.extract(connection->path);
        auto boxId = params[":boxId"];
        auto appId = params[":appId"];
        LOG_INFO << "Connection opened from " << connection->remote_endpoint().address().to_string() << " to box " << boxId
                 << " by application " << params["appId"];
        if (!Database::i().isApplicationAuthorized(appId, boxId)) {
            nlohmann::json msg;
            aquabox::to_json(msg, ErrorResponse::connectionRejected());
            connection->send(msg.dump());
            return;
        }
        if (BoxesManager::get(boxId, false) == nullptr) {
            nlohmann::json msg;
            aquabox::to_json(msg, ErrorResponse::boxIsOffline());
            connection->send(msg.dump());
            return;
        }
        LOG_INFO << "Application " << appId << " is authorized for box " << boxId;
        createConnection(connection, boxId);
    }

void ApplicationsApi::onMessage(std::shared_ptr<WSServer::Connection> connection,
                                std::shared_ptr<WSServer::InMessage> message)
{
    nlohmann::json request;
    
    try {
        request = nlohmann::json::parse(message->string());
    }
    catch (nlohmann::json::parse_error& e) {
        LOG_ERROR << "Can't parse command: " << e.what();
        sendError(connection, ErrorResponse::parseError());
        return;
    }

    auto rqid = request.find("rqid");
    if (rqid == request.end()) {
        LOG_ERROR << "No request ID in command";
        sendError(connection, ErrorResponse::rqidMissed());
        return;
    }

    if (!rqid->is_number_unsigned()) {
        LOG_ERROR << "Request id is not a number";
        sendError(connection, ErrorResponse::rqidWrongType());
        return;
    }

    auto cmd = request.find("command");
    if (cmd == request.end()) {
        LOG_ERROR << "Request does not contain command";
        return;
    }

    if (!cmd->is_string()) {
        LOG_ERROR << "Command is not a string";
        return;
    }

    Operation op = cmd->get<Operation>();
    if (Operation::Unknown == op) {
        LOG_ERROR << "Unknown command";
        sendError(connection, ErrorResponse::unknownCommand());
        return;
    }

    
    CommandRequest command = request;

    std::string boxId = command.getParameter(":boxId");
    if (boxId.empty())
    {
        LOG_ERROR << "BoxId is empty";
        sendError(connection, ErrorResponse::boxIdMissed());
        return;
    }

    std::string appId = command.getParameter(":appId");
    if (appId.empty())
    {
        LOG_ERROR << "AppId is empty";
        sendError(connection, ErrorResponse::appIdIdMissed());
        return;
    }

    std::shared_ptr<Box> box = BoxesManager::get(boxId);

    BoxResponse rsp = box->runCommandAndWait(command);

    if (rsp.type == ApiMessageType::Error)
    {
        connection->send(as_json(rsp.data.error).dump(4), Fail);
    }
    else
    {
        connection->send(as_json(rsp.data.ok).dump(4), Fail);
    }
}

void ApplicationsApi::onClose(  std::shared_ptr<WSServer::Connection> connection,
                                int status, const std::string& reason)
{
    LOG_INFO << "Closed connection from " << connection->remote_endpoint().address().to_string();
    auto params = m_rpc.extract(connection->path);
    auto boxId = params[":boxId"];
    removeConnection(boxId, connection);
}

void ApplicationsApi::onError(std::shared_ptr<WSServer::Connection> connection, SimpleWeb::error_code)
{
    LOG_INFO << "Connection from " << connection->remote_endpoint().address().to_string() << " to " << connection->path
                << " closed due error.";
    auto params = m_rpc.extract(connection->path);
    auto boxId = params[":boxId"];
    removeConnection(boxId, connection);
}

void ApplicationsApi::sendError(std::shared_ptr<WSServer::Connection> ws, const ErrorResponse& error)
{
    nlohmann::json j;
    to_json(j, error);
    ws->send(j.dump());
}