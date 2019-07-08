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