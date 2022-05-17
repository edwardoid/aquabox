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

#include "box_connection.h"
#include "enumerations.h"
#include "log.h"
#include <nlohmann/json.hpp>

using namespace aquabox;

BoxConnection::BoxConnection(const std::string& id, ResultsMap<Command>& resutsMap)
    : results(resutsMap)
    , m_id(id)
{}

const std::string& BoxConnection::id() const
{
    return m_id;
}

void BoxConnection::ready(const std::string& body)
{
    Command cmd;
    cmd.response(Result::ServerError);
    try {
        nlohmann::json data = nlohmann::json::parse(body);
        cmd.fromJSON(data);
    }
    catch (nlohmann::json::parse_error e) {
        LOG_ERROR << "Can't parse JSON from client: " << e.what();
    }
    catch (...) {
    }
    results.ready(cmd.seqNum(), cmd);
}

void BoxConnection::runCommand(Command& cmd)
{
    if (commands == nullptr) {
        cmd.response(Result::AccessDenied);
        return;
    }
    nlohmann::json response = cmd.responseJson();

    LOG_DEBUG << "Executing command\n" << response.dump();

    commands->send(response.dump());

    if (!results.waitFor(cmd.seqNum(), cmd)) {
        LOG_DEBUG << "Command execution timed out!";
        cmd.response(Result::AccessDenied);
    }
}

bool BoxConnection::empty() const
{
    return events == nullptr || commands == nullptr;
}
