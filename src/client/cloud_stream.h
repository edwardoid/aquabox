/*
 * AquaBox
 * Copyright (C) 2020  Eduard Sargsyan
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

#include <client_ws.hpp>
#include <nlohmann/json.hpp>
#include <thread>

namespace aquabox
{
    class CloudStream : SimpleWeb::SocketClient<SimpleWeb::WS>
    {
    public:
        CloudStream(const std::string& url);
        void attach();
        bool isAttached() const;
        ~CloudStream();
        virtual void sendMessage(const nlohmann::json& message);
        virtual nlohmann::json messageReceived(const nlohmann::json& message) = 0;

    protected:
        virtual void onOpen(std::shared_ptr<class Connection> connection);
        virtual void onClose(std::shared_ptr<class Connection> connection, int, const std::string&);
        virtual void onError(std::shared_ptr<class Connection>, const SimpleWeb::error_code&);

    private:
        virtual void onMessage(std::shared_ptr<class Connection> connection, std::shared_ptr<class InMessage> message);
        void send(std::string data);

    private:
        std::shared_ptr<CloudStream::Connection> m_connection;
        std::thread m_thread;
        std::atomic_bool m_reattach;
        std::string m_url;
        bool m_attached;
    };
} // namespace aquabox
