#include "webserver.h"
#include "settings.h"
#include "config.h"

using namespace aquabox;


bool WebServer::start()
{
    setupRoutes();

    m_rest = std::thread([this]() {
        m_http.config.port = Settings::integer(Settings::HTTP_PORT, Config::REST_PORT);
        m_http.config.address = "0.0.0.0";
        LOG_INFO << "Starting web server on " << m_http.config.address << ":" << m_http.config.port;
        bindApiCalls();

        m_http.start();
    });

    m_stream = std::thread([this]() {
        m_ws.config.port = Settings::integer(Settings::STREAM_PORT, Config::WS_PORT);
        m_ws.config.address = "0.0.0.0";
        LOG_INFO << "Starting websocket server" << m_ws.config.address << ":" << m_ws.config.port;

        ws::API::Ping(m_ws, m_updates);

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

    m_createRule = base.sub().word("device").string(":dev").word("rule");

    m_updateRule = base.sub().word("device").string(":dev").word("rule");

    m_turnOn = base.sub().word("device").string(":dev").word("turn_on");

    m_turnOff = base.sub().word("device").string(":dev").word("turn_off");

    m_rulesForDevice = base.sub().word("rules").string(":dev");

    m_repeatUnits = base.sub().word("config").word("repeat_units");

    m_wifi.word("wifi");

    m_updates = base.sub().word("updates");
}

bool WebServer::bindApiCalls()
{
    m_http.resource[m_static.url()]["GET"] = std::bind(&rest::API::StaticFiles, &m_API, m_static, std::placeholders::_1, std::placeholders::_2);
    m_http.resource[m_login.url()]["GET"] = std::bind(&rest::API::Login, &m_API, m_login, std::placeholders::_1, std::placeholders::_2);
    m_http.resource[m_status.url()]["GET"] = std::bind(&rest::API::Status, &m_API, m_status, std::placeholders::_1, std::placeholders::_2);
    m_http.resource[m_statusQR.url()]["GET"] = std::bind(&rest::API::StatusQR, &m_API, m_statusQR, std::placeholders::_1, std::placeholders::_2);
    m_http.resource[m_allDevices.url()]["GET"] = std::bind(&rest::API::Devices, &m_API, m_allDevices, std::placeholders::_1, std::placeholders::_2);
    m_http.resource[m_deviceById.url()]["GET"] = std::bind(&rest::API::GetDevice, &m_API, m_deviceById, std::placeholders::_1, std::placeholders::_2);

    m_http.resource[m_allRules.url()]["GET"] = std::bind(&rest::API::Rules, &m_API, m_allRules, std::placeholders::_1, std::placeholders::_2);
    m_http.resource[m_ruleById.url()]["GET"] = std::bind(&rest::API::GetRule, &m_API, m_ruleById, std::placeholders::_1, std::placeholders::_2);

    m_http.resource[m_deleteRule.url()]["DELETE"] = std::bind(&rest::API::DeleteRule, &m_API, m_ruleById, std::placeholders::_1, std::placeholders::_2);
    m_http.resource[m_deleteRule.url()]["OPTIONS"] = std::bind(&rest::API::OptionsOK, &m_API, m_ruleById, std::placeholders::_1, std::placeholders::_2);
    
    m_http.resource[m_createRule.url()]["PUT"] = std::bind(&rest::API::CreateRule, &m_API, m_createRule, std::placeholders::_1, std::placeholders::_2);
    m_http.resource[m_createRule.url()]["OPTIONS"] = std::bind(&rest::API::OptionsOK, &m_API, m_ruleById, std::placeholders::_1, std::placeholders::_2);

    m_http.resource[m_updateRule.url()]["POST"] = std::bind(&rest::API::CreateRule, &m_API, m_updateRule, std::placeholders::_1, std::placeholders::_2);
    m_http.resource[m_updateRule.url()]["OPTIONS"] = std::bind(&rest::API::OptionsOK, &m_API, m_ruleById, std::placeholders::_1, std::placeholders::_2);
    
    m_http.resource[m_rulesForDevice.url()]["GET"] = std::bind(&rest::API::DeviceRules, &m_API, m_rulesForDevice, std::placeholders::_1, std::placeholders::_2);
    m_http.resource[m_repeatUnits.url()]["GET"] = std::bind(&rest::API::SupportedRepeatTimeouts, &m_API, m_repeatUnits, std::placeholders::_1, std::placeholders::_2);
    m_http.resource[m_wifi.url()]["GET"] = std::bind(&rest::API::WiFiQR, &m_API, m_wifi, std::placeholders::_1, std::placeholders::_2);

    m_http.resource[m_turnOn.url()]["GET"] = std::bind(&rest::API::TurnOn, &m_API, m_turnOn, std::placeholders::_1, std::placeholders::_2);
    m_http.resource[m_turnOff.url()]["GET"] = std::bind(&rest::API::TurnOff, &m_API, m_turnOff, std::placeholders::_1, std::placeholders::_2);
}
