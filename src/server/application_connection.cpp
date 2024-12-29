/*
 * AquaBox
 * Copyright (C) 2022  Eduard Sargsyan
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

#include "application_connection.h"
#include <common/log.h>
#include <common/singleton.h>
#include <types/error_response.h>
#include <types/event.h>
#include <types/command.h>
#include "hardware_api.h"

using namespace aquabox;

std::function<void(const SimpleWeb::error_code &)> Fail([](const SimpleWeb::error_code& e) {
    if (e) LOG_WARNING << "Write to ws filed with: " << e.message() << " (code: " << e.value() << ")";
});

ApplicationConnection::ApplicationConnection(ApplicationConnectionPtr app)
    : m_app(app)
{}

bool ApplicationConnection::interested(const std::string& boxId) const
{
    return m_boxes.count(boxId);
}

void ApplicationConnection::subscribe(const std::string& boxId)
{
    m_boxes.insert({boxId, {}});
}

void ApplicationConnection::unsubscribe(const std::string& boxId)
{
    m_boxes.erase({boxId, {}});
}

void ApplicationConnection::run(const CommandRequest& command)
{
    std::string boxId = command.getParameter(":boxId");
    std::shared_ptr<BoxConnection> box = HardwareApi::box(boxId);
    if (box == nullptr) {
        CommandResult res = CommandResult::createResult(command);
        res.setData("on_cloud");
        handle(boxId, ErrorResponse::boxIsOffline(command.rqid), command.rqid);
        return;
    }
    m_boxes[boxId].insert(command.rqid);
    box->runCommand(command);
}

bool ApplicationConnection::handle(const std::string& boxId, const CommandResult& cmd)
{
    auto p = m_boxes.find(boxId);
    if ( p != m_boxes.end() )
    {
        auto r = p->second.find(cmd.rpid);
        if (r != p->second.end()) {
            return send(as_json(cmd));
        }
    }
    return false;
}

bool ApplicationConnection::handle(const std::string& boxId, const Event& event)
{
    auto p = m_boxes.find(boxId);
    if ( p != m_boxes.end() )
    {
        return send(as_json(event));
    }
    return false;
}

bool ApplicationConnection::handle(const std::string& boxId, const ErrorResponse& error, uint64_t rpId)
{
    auto p = m_boxes.find(boxId);
    if ( p != m_boxes.end() )
    {
        auto r = p->second.find(error.rpid);
        if (r != p->second.end()) {
            ErrorResponse e;
            e.boxId = boxId;
            e.rpid = rpId;
            return send(as_json(e));
        }
    }
    return false;
}

bool ApplicationConnection::send(const nlohmann::json& data)
{
    if (m_app.expired()) {
        return false;
    }

    m_app.lock()->send(data.dump(), Fail);
    return true;
}