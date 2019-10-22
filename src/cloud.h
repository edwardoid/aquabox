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

#ifndef CLOUD_H
#define CLOUD_H

#include <client_http.hpp>
#include <unordered_map>
#include <memory>
#include "cloud_stream.h"

using WsClient = SimpleWeb::SocketClient<SimpleWeb::WS>;
using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;

namespace aquabox
{
    class Cloud
    {
    public:
        void connect();
        void disconnect();
        bool isRegistered();
        void doRegister();
        void heartheat();
    private:
        bool isConnected();
    private:
        std::unordered_map<std::string, std::shared_ptr<CloudStream>> m_streams;
    };
}

#endif // CLOUD_H