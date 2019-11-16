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

#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <thread>
#include <server_http.hpp>
#include <server_ws.hpp>
#include "route.h"
#include "rest_api.h"
#include "ws_api.h"

class Event;

namespace aquabox
{
    class WebServer
    {
    public:
        bool start();
        void setupRoutes();
        bool bindApiCalls();
        void postUpdate(std::shared_ptr<Event> event);
    private:
        using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;

        SimpleWeb::Server<SimpleWeb::HTTP> m_http;
        WsServer m_ws;
        std::thread m_rest;
        std::thread m_stream;
        rest::API REST;
        ws::API WS;
        Route m_updates;
        Route m_rpc;
        Route m_static;
        Route m_login;
        Route m_status;
        Route m_statusQR;
        Route m_allDevices;
        Route m_deviceById;
        Route m_updateDeviceById;
        Route m_allRules;
        Route m_ruleById;
        Route m_createRule;
        Route m_deleteRule;
        Route m_updateRule;
        Route m_rulesForDevice;
        Route m_repeatUnits;
        Route m_wifiScan;
        Route m_wifiConnect;

        Route m_turnOn;
        Route m_turnOff;
    };
}

#endif // WEB_SERVER_H