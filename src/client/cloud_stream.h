/*
 * AquaBox
 * Copyright (C) 2020-2022  Eduard Sargsyan
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
        enum Status
        {
            Detached = 0,
            Attaching,
            Attached
        };
        CloudStream();
        void attach();
        ~CloudStream();
        void checkIfRegistered();
        bool isRegistered();
        void postUpdate(std::shared_ptr<class Event> event);
        virtual Status status() const { return m_status; }
        virtual void sendMessage(const nlohmann::json& message);
        virtual void messageReceived(const nlohmann::json& message);

    protected:
        virtual void onOpen(std::shared_ptr<class Connection> connection);
        virtual void onClose(std::shared_ptr<class Connection> connection, int, const std::string&);
        virtual void onError(std::shared_ptr<class Connection>, const SimpleWeb::error_code&);

    private:
        virtual void onMessage(std::shared_ptr<class Connection> connection, std::shared_ptr<class InMessage> message);
        void send(const std::string data);
        nlohmann::json exec(const nlohmann::json& message);

        static std::string url();

    private:
        std::shared_ptr<CloudStream::Connection> m_connection;
        std::thread m_thread;
        std::atomic_bool m_reattach;
        Status m_status;
    };

    using Cloud = CloudStream;
} // namespace aquabox
