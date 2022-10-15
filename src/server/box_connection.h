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

#pragma once

#include "results_map.h"
#include <types/command.h>
#include <types/error_response.h>
#include <memory>
#include <server_ws.hpp>
#include <string>
#include <unordered_map>

namespace aquabox
{
    struct BoxResponse
    {
        BoxResponse() = default; 
        BoxResponse(const BoxResponse& src)
        {
            this->type = src.type;
            this->data = src.data;
        }
        ApiMessageType type;
        struct BoxResponseData
        {
            ErrorResponse error;
            CommandResult ok;
        } data;
    };

    class BoxConnection
    {
    public:

        BoxConnection(const std::string& id, ResultsMap<BoxResponse>& results);
        using WSServer = SimpleWeb::SocketServer<SimpleWeb::WS>;
        const std::string& id() const;
        bool empty() const;
        void ready(const std::string& data);
        void runCommand(const CommandRequest& cmd);
        BoxResponse runCommandAndWait(CommandRequest& cmd);
        ResultsMap<BoxResponse>& results;
        std::shared_ptr<WSServer::Connection> commands;

    private:
        std::string m_id;
    };
} // namespace aquabox
