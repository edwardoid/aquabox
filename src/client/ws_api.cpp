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

#include "ws_api.h"
#include "command_executor.h"
#include <types/command.h>
#include <common/enumerations.h>
#include <types/event.h>

using namespace aquabox::ws;


static std::function<void(const Error &)> Fail([](const Error& e) {
    if (e) LOG_WARNING << "Write to ws filed with: " << e.message() << " (code: " << e.value() << ")";
});

void API::onOpen(const std::string& route, Connection connection)
{
    std::lock_guard<std::mutex> guard(m_lock);

    auto connections = m_connections.find(route);
    if (connections == m_connections.end()) {
        m_connections.insert({route, std::vector<Connection>({connection})});
    }
    else {
        connections->second.push_back(connection);
    }
}
void API::onClose(const std::string& route, Connection connection, Status status, const std::string& reason)
{
    std::lock_guard<std::mutex> guard(m_lock);

    auto connections = m_connections.find(route);
    if (connections == m_connections.end()) {
        return;
    }

    for (auto i = connections->second.begin(); i != connections->second.end(); ++i) {
        if (i->get() == connection.get()) {
            connections->second.erase(i);
            break;
        }
    }
}

void API::postEvent(Event event)
{
    std::lock_guard<std::mutex> guard(m_lock);

    auto connections = m_connections.find(CommandsUrl);
    if (connections == m_connections.end()) {
        return;
    }

    nlohmann::json msg = *event;
    
    for (auto& connection : connections->second) {
        connection->send(msg.dump(4), Fail);
    }
}

DEFINE_WS_API_IMPL(Updates)
{
    connection->send("Updates", Fail);
}

DEFINE_WS_API_IMPL(Commands)
{
    nlohmann::json request;
    try {
        request = nlohmann::json::parse(response->string());
    }
    catch (nlohmann::json::parse_error& e) {
        LOG_ERROR << "Can't parse command: " << e.what();
        ErrorResponse err = ErrorResponse::parseError();
        err.setDetails(e.what());
        connection->send(as_json(err).dump(4), Fail);
        return;
    }

    auto rqid = request.find("rqid");
    if (rqid == request.end()) {
        LOG_ERROR << "Request id is not specified";
        connection->send(as_json(ErrorResponse::rqidMissed()).dump(4), Fail);
        return;
    }

    if (!rqid->is_number_unsigned()) {
        LOG_ERROR << "Request id is not a number";
        connection->send(as_json(ErrorResponse::rqidWrongType()).dump(4), Fail);
        return;
    }

    auto cmd = request.find("command");
    if (cmd == request.end()) {
        LOG_ERROR << "Request does not contain command";
        connection->send(as_json(ErrorResponse::requestIsNotCommand()).dump(4), Fail);
        return;
    }

    if (!cmd->is_string()) {
        LOG_ERROR << "Command is not a string";
        connection->send(as_json(ErrorResponse::parseError()).dump(4), Fail);
        return;
    }

    Operation op = cmd->get<Operation>();
    if (Operation::Unknown == op) {
        LOG_ERROR << "Unknown command";
        connection->send(as_json(ErrorResponse::unknownCommand()).dump(4), Fail);
        return;
    }

    auto body = request.find("data");

    CommandRequest command;
    from_json(request, command);

    CommandExecutor exec;
    ExecutionResult result = exec.exec(command);
    if (result) {
        LOG_DEBUG << "Command " << (int) (command.operation()) << " finished with " << (int) (result.fail.result());
    }
    else {
        LOG_ERROR << "Can't execute command! " << (int) (command.operation());
    }

    try {
        connection->send(as_json(result).dump(4), Fail);
    }
    catch (...) {
        LOG_ERROR << "Response serialization failed!";
    }
}