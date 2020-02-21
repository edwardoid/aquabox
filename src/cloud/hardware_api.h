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

#ifndef HARDWARE_API_H
#define HARDWARE_API_H

#include <string>
#include <memory>
#include <server_http.hpp>
#include <server_ws.hpp>
#include "route.h"

namespace aquabox
{
    class HardwareApi
    {
        using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
        using WSServer = SimpleWeb::SocketServer<SimpleWeb::WS>;

        HardwareApi() {};
        HardwareApi(const HardwareApi&) = delete;
        HardwareApi(HardwareApi&&) = delete;
        HardwareApi& operator = (const HardwareApi&) = delete;
        HardwareApi& operator = (HardwareApi&&) = delete;
    public:
        static bool start();
    private:
        bool acceptConnectionFromHW(std::string id, std::shared_ptr<WSServer::Connection> connection);
        void setupRoutes();
    private:
        static HardwareApi self;
        HttpServer m_http;
        WSServer m_ws;
        Route m_register;
        Route m_login;
        Route m_events;
        Route m_commands;
    };
}


#endif // HARDWARE_API_H
