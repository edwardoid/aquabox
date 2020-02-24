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

#include "api.h"

#define Access_Control_Allow_Origin "Access-Control-Allow-Origin"
#define Access_Control_Allow_Headers "Access-Control-Allow-Headers"
#define Access_Control_Allow_Methods "Access-Control-Allow-Methods"

using namespace aquabox;
using namespace aquabox::v1;

void API::init(std::shared_ptr<HttpServer> http, Route base)
{
    m_server = http;

    m_status = base.sub().word("status");

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

    if (m_allowCORS) {
        m_server->default_resource["OPTIONS"] = std::bind(&API::CORSOptions, this, std::placeholders::_2, std::placeholders::_1);
        m_server->default_resource["GET"] = std::bind(&API::CORSOptions, this, std::placeholders::_2, std::placeholders::_1);
        m_server->default_resource["POST"] = std::bind(&API::CORSOptions, this, std::placeholders::_2, std::placeholders::_1);
        m_server->default_resource["PUT"] = std::bind(&API::CORSOptions, this, std::placeholders::_2, std::placeholders::_1);
        m_server->default_resource["DELETE"] = std::bind(&API::CORSOptions, this, std::placeholders::_2, std::placeholders::_1);
    }

    setup("GET", m_status, &API::Status);
    setup("GET", m_allDevices, &API::Devices);
    setup("GET", m_deviceById, &API::GetDevice);
    setup("POST", m_updateDeviceById, &API::UpdateDevice);
    setup("GET", m_allRules, &API::Rules);
    setup("GET", m_ruleById, &API::GetRule);
    setup("DELETE", m_deleteRule, &API::DeleteRule);
    setup("PUT", m_createRule, &API::CreateRule);
    setup("POST", m_updateRule, &API::CreateRule);
    setup("GET", m_rulesForDevice, &API::DeviceRules);
    setup("GET", m_repeatUnits, &API::SupportedRepeatTimeouts);
    setup("GET", m_wifiScan, &API::ScanWiFiNetworks);
    setup("POST", m_wifiConnect, &API::ConnectToWiFi);
    setup("PUT", m_turnOn, &API::TurnOn);
    setup("PUT", m_turnOff, &API::TurnOff);
}

void API::defaultHandler(const Route& route, Request& request, Response& response, Headers& headers)
{

}

void API::CORSOptions(Web::Request request, Web::Response response)
{
    Headers headers;
    addCORSHeaders(headers);
    response->write(SimpleWeb::StatusCode::success_ok, headers);
}

void API::addCORSHeaders(Web::Headers& headers)
{
    headers.emplace("Content-Type", "application/json");
    headers.emplace(Access_Control_Allow_Origin, "*");
    headers.emplace(Access_Control_Allow_Headers, "Origin, X-Requested-With, Content-Type, Accept");
    headers.emplace(Access_Control_Allow_Methods, "POST, GET, PUT, PATCH, DELETE, OPTIONS");                         
}

void API::enableCORS(bool enable)
{
    m_allowCORS = enable;
}

void API::setup(std::string method, const Route& route, void (API::* Callback)(const Route&, Request&, Response&, Headers&))
{
    m_server->resource[route.url()][method] = std::bind(&API::dispatcher, this, route, Callback,
                                                        std::placeholders::_2,
                                                        std::placeholders::_1);
    if (m_allowCORS) {
        m_server->resource[route.url()]["OPTIONS"] = std::bind(&API::CORSOptions, this,
                                                        std::placeholders::_2,
                                                        std::placeholders::_1);
    }
}

void API::dispatcher(const Route& route, void (API::* Callback)(const Route&, Request&, Response&, Headers&), Request request, Response response)
{
    Headers headers;
    if (m_allowCORS) {
        addCORSHeaders(headers);
    }

    (this ->* Callback)(route, request, response, headers);
}