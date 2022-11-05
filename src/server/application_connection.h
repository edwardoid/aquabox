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

#pragma once

#include <server_ws.hpp>
#include "api_calls_handler.h"
#include <unordered_map>
#include <set>
#include <nlohmann/json.hpp>
#include <memory>

namespace aquabox
{
    class CommandRequest;

    using WSConnectionImpl = SimpleWeb::SocketServer<SimpleWeb::WS>::Connection;
    using ApplicationConnectionRawPtr = WSConnectionImpl*;
    using ApplicationConnectionPtr = std::weak_ptr<WSConnectionImpl>;

    class ApplicationConnection: public ApiCallsHandler
    {
    public:
        ApplicationConnection(ApplicationConnectionPtr app);

        inline bool operator == (const ApplicationConnection& other ) const { return *this == other.m_app; }
        inline bool operator == (const ApplicationConnectionPtr& other ) const { return m_app.lock() == other.lock(); }

        bool interested(const std::string& boxId) const;
        void subscribe(const std::string& boxId);
        void unsubscribe(const std::string& boxId);

        void run(const CommandRequest& command);

        virtual bool handle(const std::string& boxId, const CommandResult& command) override;
        virtual bool handle(const std::string& boxId, const Event& event) override;
        virtual bool handle(const std::string& boxId, const ErrorResponse& error, uint64_t rpid) override;
    private:
        bool send(const nlohmann::json& data);
    private:
        /* Marring BoxId to requests id. */
        std::unordered_map<std::string, std::set<uint64_t>> m_boxes;

        /* WS connection reference. */
        ApplicationConnectionPtr m_app;
    };
}