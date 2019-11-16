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

#ifndef CLOUD_STREAM_H
#define CLOUD_STREAM_H

#include <client_ws.hpp>
#include <thread>

namespace aquabox
{
    class CloudStream: SimpleWeb::SocketClient<SimpleWeb::WS>
    {
    public:
        CloudStream(std::string url);
        void attach();
        bool isAttached() const;
        void detach();
        void send(std::string data);
        ~CloudStream();
    protected:
        virtual void onOpen(std::shared_ptr<class Connection> connection);
        virtual void onMessage(std::shared_ptr<class Connection> connection, std::shared_ptr<class InMessage> message);
        virtual void onClose(std::shared_ptr<class Connection> connection, int, const std::string &);
        virtual void onError(std::shared_ptr<class Connection>, const SimpleWeb::error_code &);
    private:
        std::shared_ptr<CloudStream::Connection> m_connection;
        std::thread m_thread;
        std::string m_url;
        bool m_attached;
    };
}

#endif // CLOUD_STREAM