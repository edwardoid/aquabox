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

BoxConnection::BoxConnection(const std::string& id, ResultsMap<BoxResponse>& resutsMap)
    : results(resutsMap)
    , m_id(id)
{}

const std::string& BoxConnection::id() const
{
    return m_id;
}

void BoxConnection::ready(const std::string& body)
{
    try {
        BoxResponse resp;
        resp.type = ApiMessageType::Error;
        resp.data.error = ErrorResponse::parseError();
        nlohmann::json raw = nlohmann::json::parse(body);
        BaseStreamMessage result = raw;

        resp.type = result.type;
        if (result.type == ApiMessageType::Event)
        {
            LOG_INFO << "Got an event";
        }
        else if (result.type == ApiMessageType::Command)
        {
            resp.data.ok = raw;
            results.ready(resp.data.ok.rpid, resp);
        }
        else
        {
            resp.data.error = raw;
            results.ready(resp.data.error.rpid, resp);
        }
    }
    catch (nlohmann::json::parse_error e) {
        LOG_ERROR << "Can't parse JSON from client: " << e.what();
    }
    catch (...) {
    }
}

void BoxConnection::runCommand(const CommandRequest& cmd)
{
    nlohmann::json response = as_json(cmd);

    LOG_DEBUG << "Executing command\n" << response.dump();

    commands->send(response.dump());
}

BoxResponse BoxConnection::runCommandAndWait(CommandRequest& cmd)
{
    BoxResponse resp;
    resp.type = ApiMessageType::Error;
    if (commands == nullptr) {
        resp.data.error = ErrorResponse::boxIsOffline();
        resp.data.error.rpid = cmd.rqid;
        return resp;
    }
    runCommand(cmd);
    if (!results.waitFor(cmd.rqid, resp)) {
        LOG_DEBUG << "Command execution timed out!";
        resp.data.error = ErrorResponse::boxIsOffline();
        resp.data.error.rpid = cmd.rqid;
    }
    return resp;
}

bool BoxConnection::empty() const
{
    return commands == nullptr;
}
