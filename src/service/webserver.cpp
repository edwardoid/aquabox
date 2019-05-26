#include "webserver.h"
#include "settings.h"
#include "config.h"

using namespace aquabox;


bool WebServer::start()
{
    setupRoutes();

    m_rest = std::thread([this]() {
        m_http.config.port = Settings::integer(Settings::HTTP_PORT, Config::REST_PORT);
        LOG_INFO << "Starting web server on " << m_http.config.address << ":" << m_http.config.port;
        bindApiCalls();

        m_http.start();
    });

    m_stream = std::thread([this]() {
        m_ws.config.port = Settings::integer(Settings::STREAM_PORT, Config::WS_PORT);
        LOG_INFO << "Starting websocket server" << m_ws.config.address << ":" << m_ws.config.port;

        WS.Ping(m_ws, m_updates);

        m_ws.start();
    });

    m_rest.join();
    m_stream.join();
}

void WebServer::setupRoutes()
{
    m_static.word("static").any();

    const Route base = Route().word("api").word("v1");

    m_login = base.sub().word("login");

    m_status = base.sub().word("status");

    m_statusQR = base.sub().word("status").word("qr");

    m_allDevices = base.sub().word("devices");

    m_allRules = base.sub().word("rules");

    m_ruleById = base.sub().word("rule").string(":rule");

    m_deleteRule = base.sub().word("rule").string(":rule");

    m_deviceById = base.sub().word("device").string(":dev");

    m_updateDeviceById = base.sub().word("device").string(":dev");

    m_createRule = base.sub().word("device").string(":dev").word("rule");

    m_updateRule = base.sub().word("device").string(":dev").word("rule");

    m_turnOn = base.sub().word("device").string(":dev").word("turn_on");

    m_turnOff = base.sub().word("device").string(":dev").word("turn_off");

    m_rulesForDevice = base.sub().word("rules").string(":dev");

    m_repeatUnits = base.sub().word("config").word("repeat_units");

    m_wifiScan = base.sub().word("wifi").word("scan");

    m_wifiConnect = base.sub().word("wifi").word("connect");

    m_updates = base.sub().word("updates");
}

bool WebServer::bindApiCalls()
{
    REST.init(m_http);
    REST.setup(m_http, "GET", m_static, &rest::API::StaticFiles);
    REST.setup(m_http, "GET", m_login, &rest::API::Login);
    REST.setup(m_http, "GET", m_status, &rest::API::Status);
    REST.setup(m_http, "GET", m_statusQR, &rest::API::StatusQR);
    REST.setup(m_http, "GET", m_allDevices, &rest::API::Devices);
    REST.setup(m_http, "GET", m_deviceById, &rest::API::GetDevice);
    REST.setup(m_http, "POST", m_updateDeviceById, &rest::API::UpdateDevice);
    REST.setup(m_http, "GET", m_allRules, &rest::API::Rules);
    REST.setup(m_http, "GET", m_ruleById, &rest::API::GetRule);
    REST.setup(m_http, "DELETE", m_deleteRule, &rest::API::DeleteRule);
    REST.setup(m_http, "PUT", m_createRule, &rest::API::CreateRule);
    REST.setup(m_http, "POST", m_updateRule, &rest::API::CreateRule);
    REST.setup(m_http, "GET", m_rulesForDevice, &rest::API::DeviceRules);
    REST.setup(m_http, "GET", m_repeatUnits, &rest::API::SupportedRepeatTimeouts);
    REST.setup(m_http, "GET", m_wifiScan, &rest::API::ScanWiFiNetworks);
    REST.setup(m_http, "POST", m_wifiConnect, &rest::API::ConnectToWifi);
    REST.setup(m_http, "PUT", m_turnOn, &rest::API::TurnOn);
    REST.setup(m_http, "PUT", m_turnOff, &rest::API::TurnOff);
}

void WebServer::postUpdate(std::shared_ptr<Event> event)
{
    WS.postEvent(event);
}