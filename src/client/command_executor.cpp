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

#include "command_executor.h"
#include "aquabox.h"
#include "devices_map.h"
#include "settings.h"
#include "device.h"
#include "rule.h"
#include "events/update_events.h"
#include "networking/network_manager.h"
#include "cloud.h"
#include <boost/asio/ip/host_name.hpp>

using namespace aquabox;

bool CommandExecutor::exec(const Command &cmd)
{
    m_cmd = cmd;
    bool ok = false;
    switch (cmd.operation())
    {
    case Operation::GetStatus:
    {
        ok = GetStatus();
        break;
    }
    case Operation::GetDevices:
    {
        ok = GetDevices();
        break;
    }
    case Operation::GetDevice:
    {
        ok = GetDevice();
        break;
    }
    case Operation::UpdateDevice:
    {
        ok = UpdateDevice();
        break;
    }
    case Operation::GetRules:
    {
        ok = GetRules();
        break;
    }
    case Operation::GetRule:
    {
        ok = GetRule();
        break;
    }
    case Operation::CreateRule:
    {
        ok = CreateOrUpdateRule(false);
        break;
    }
    case Operation::UpdateRule:
    {
        ok = CreateOrUpdateRule(true);
        break;
    }
    case Operation::DeleteRule:
    {
        ok = DeleteRule();
        break;
    }
    case Operation::DeviceRules:
    {
        ok = DeviceRules();
        break;
    }
    case Operation::SupportedRepeatTimeouts:
    {
        ok = SupportedRepeatTimeouts();
        break;
    }
    case Operation::Set:
    {
        ok = SetValue();
        break;
    }
    case Operation::ScanWiFiNetworks:
    {
        ok = ScanWiFiNetworks();
        break;
    }
    case Operation::WiFiNetworks:
    {
        ok = ListNetworks();
        break;
    }
    case Operation::ConnectToWiFi:
    {
        ok = ConnectToWiFi();
        break;
    }
    default:
    {
        LOG_ERROR << "Unsupported operation " << (int)(cmd.operation());
    }
    }
    const_cast<Command &>(cmd) = m_cmd;
    return ok;
}

bool CommandExecutor::GetStatus()
{
    nlohmann::json host({{"rest", Settings::integer(Settings::Keys::HTTP_PORT, Settings::Defaults::REST_PORT)},
                         {"stream", Settings::integer(Settings::Keys::STREAM_PORT, Settings::Defaults::WS_PORT)},
                         {"serial", Settings::string(Settings::Keys::SERIAL, Settings::Defaults::SERIAL)}});

    auto nm = Singleton<NetworkManager>::i();

    if (nm != nullptr)
    {
        host["network"] = nlohmann::json({{"ssid", nm->currentConnection.value.ssid},
                                          {"ip", nm->currentConnection.value.ip},
                                          {"internet", nm->connectedToInternet()}});
    }

    host["host"] = boost::asio::ip::host_name();

    host["config"] = Singleton<AquaBox>::get()->configurationData();

    auto cloud = Singleton<Cloud>::i();
    bool isRegistered = cloud == nullptr ? false : (bool)cloud->isRegistered;

    m_cmd.response(nlohmann::json({{"aquabox", host}, {"status", "online"}, {"registered", isRegistered}}), Result::Ok);
    return true;
}

bool CommandExecutor::GetDevices()
{
    std::vector<nlohmann::json> all;
    Singleton<DevicesMap>::get()->for_each([this, &all](auto key, auto dev) {
        nlohmann::json json = *dev;
        for (auto p : dev->properties()) {
            json[p] = dev->value(p);
        }
        all.push_back(json);
    });

    m_cmd.response(nlohmann::json({{"devices", all}}), Result::Ok);
    return true;
}

bool CommandExecutor::GetDevice()
{
    if (!m_cmd.hasParameter(":dev"))
    {
        LOG_ERROR << "No device id parameter in URL";
        m_cmd.response(Result::BadCommand);
        return true;
    }

    std::string id = m_cmd.getParameter(":dev");

    auto dev = Singleton<DevicesMap>::i()->get(id);
    if (dev == nullptr)
    {
        LOG_ERROR << "No device " << dev << " not found";
        m_cmd.response(Result::NotFound);
        return true;
    }

    nlohmann::json data = *dev;
    for (auto p : dev->properties()) {
        data[p] = dev->value(p);
    }

    m_cmd.response(data, Result::Ok);
    return true;
}

bool CommandExecutor::UpdateDevice()
{
    if (!m_cmd.hasParameter(":dev"))
    {
        LOG_ERROR << "No device id parameter in URL";
        m_cmd.response(Result::BadCommand);
        return true;
    }

    std::string id = m_cmd.getParameter(":dev");

    std::shared_ptr<Device> d = std::make_shared<Device>();
    try
    {
        *d = m_cmd.request();
    }
    catch (nlohmann::detail::parse_error &e)
    {
        nlohmann::json data;
        data["request"] = m_cmd.request();
        data["error"] = e.what();
        data["cause"] = "Parse Error";
        m_cmd.response(data, Result::BadCommand);
        return true;
    }

    bool found = Singleton<DevicesMap>::i()->on(id, [this, d](auto id, auto dev) {
        nlohmann::json before = *dev;
        for (auto& p : dev->properties())
        {
            before[p] = dev->value(p);
        }

        LOG_DEBUG << "Updating device " << id;
        *dev = *d;

        auto box = Singleton<AquaBox>::get();
        box->updateDevice(id);
        box->postUpdate(std::make_shared<DeviceUpdatedEvent>(dev, before));

        m_cmd.response(Result::Ok);
    });

    if (!found)
    {
        LOG_ERROR << "No device " << id << " not found";
        m_cmd.response(Result::NotFound);
        return true;
    }

    return true;
}

bool CommandExecutor::GetRules()
{
    RulesMap &allRules = Singleton<AquaBox>::get()->rules();
    std::vector<nlohmann::json> all;
    allRules.for_each([&all](auto id, auto rule) {
        nlohmann::json data = *rule;
        all.push_back(data);
    });
    nlohmann::json res = nlohmann::json::object();
    res["rules"] = all;
    m_cmd.response(nlohmann::json({{"rules", all}}), Result::Ok);
    return true;
}

bool CommandExecutor::GetRule()
{
    if (!m_cmd.hasParameter(":rule"))
    {
        LOG_ERROR << "No rule id parameter in URL";
        m_cmd.response(Result::BadCommand);
        return true;
    }
    std::string id = m_cmd.getParameter(":rule");

    RulesMap &all = Singleton<AquaBox>::get()->rules();

    auto rule = all.get(id, nullptr);

    if (rule == nullptr)
    {
        m_cmd.response(Result::NotFound);
    }
    else
    {
        nlohmann::json data = *rule;
        m_cmd.response(data, Result::Ok);
    }
    return true;
}

bool CommandExecutor::CreateOrUpdateRule(bool update)
{
    if (!m_cmd.hasParameter(":dev"))
    {
        LOG_ERROR << "No device id parameter in URL";
        m_cmd.response(Result::BadCommand);
        return true;
    }
    std::string dev = m_cmd.getParameter(":dev");
    auto r = std::make_shared<Rule>();
    try
    {
        *r = m_cmd.request();
    }
    catch (nlohmann::detail::parse_error &e)
    {
        LOG_ERROR << "Rule can not be parsed because: " << e.what();
        m_cmd.response( e.what(), Result::BadCommand);
        return true;
    }

    if (r->resolveDevice() == nullptr)
    {
        m_cmd.response( "Can't find device", Result::BadCommand);
        return true;
    }

    if (!Singleton<AquaBox>::get()->addRule(r, update))
    {
        m_cmd.response("Wrong method or internal data", Result::BadCommand);
        return true;
    }

    m_cmd.response(m_cmd.request(), Result::Ok);
    return true;
}

bool CommandExecutor::DeleteRule()
{
    if (!m_cmd.hasParameter(":rule"))
    {
        LOG_ERROR << "No rule id parameter in URL";
        m_cmd.response(Result::BadCommand);
        return true;
    }
    auto id = m_cmd.getParameter(":rule");
    if (Singleton<AquaBox>::get()->removeRule(id))
    {
        LOG_DEBUG << "Rule " << id << " removed";
        m_cmd.response(Result::Ok);
    }
    else
    {
        LOG_ERROR << "Rule " << id << " removal failed";
        m_cmd.response(Result::ServerError);
    }
    return true;
}

bool CommandExecutor::DeviceRules()
{
    if (!m_cmd.hasParameter(":dev"))
    {
        LOG_ERROR << "No dev id parameter in URL";
        m_cmd.response(Result::BadCommand);
        return true;
    }
    std::string dev = m_cmd.getParameter(":dev");
    std::vector<nlohmann::json> all;
    RulesMap &allRules = Singleton<AquaBox>::get()->rules();

    allRules.for_each([&all, &dev](auto id, auto rule) {
        if (rule->device() == dev)
        {
            nlohmann::json data = *rule;
            all.push_back(data);
        }
    });

    nlohmann::json res({{"rules", all},
                        {"device", dev}});
    m_cmd.response(res, Result::Ok);
    return true;
}

bool CommandExecutor::SupportedRepeatTimeouts()
{
    nlohmann::json res;
    res["units"] = std::vector<std::string>({ "hour" });
    m_cmd.response(res, Result::Ok);
    return true;
}

bool CommandExecutor::SetValue()
{
    if (!m_cmd.hasParameter(":dev"))
    {
        LOG_ERROR << "No dev id parameter in URL";
        m_cmd.response(Result::BadCommand);
        return true;
    }

    auto changes = m_cmd.request()["changes"];
    if (!changes.is_array()) {
        LOG_ERROR << "Set value should be array";
    }

    std::string id = m_cmd.getParameter(":dev");
    auto dev = Singleton<DevicesMap>::i()->get(id);
    if (dev == nullptr)
    {
        m_cmd.response(Result::NotFound);
        return true;
    }

    nlohmann::json data = *dev;
    for (auto v : changes) {
        std::string property = v["property"].get<std::string>();
        LOG_ERROR << "Setting value for " << property << " to be " << v["value"].dump();
        dev->setValue(property, v["value"]);
        data[property] = dev->value(property);
        Singleton<AquaBox>::get()->postUpdate(std::make_shared<ValueChangedEvent>(dev, property, "user"));
    }

    m_cmd.response(data, Result::Ok);
    return true;
}

bool CommandExecutor::ScanWiFiNetworks()
{
    auto nm = Singleton<NetworkManager>::i();

    bool res = false;
    if (nm != nullptr)
    {
        res = nm->requestScan();
    }

    m_cmd.response(nlohmann::json({{"requestResult", res}}), Result::Ok);
    return true;
}

bool CommandExecutor::ListNetworks()
{
    std::vector<nlohmann::json> nets;

    auto nm = Singleton<NetworkManager>::i();

    if (nm != nullptr)
    {
        auto currentConnection = nm->currentConnection.value;
        for (auto &net : nm->networks())
        {
            nets.emplace_back(nlohmann::json(
                {{"ssid", net.ssid},
                 {"ip", (net.ssid == currentConnection.ssid ? currentConnection.ip : "")},
                 {"auth", (net.auth == nm::Security::WEP ? "wep" : (net.auth == nm::Security::WPA ? "wpa" : (net.auth == nm::Security::WPA2 ? "wpa2" : (net.auth == nm::Security::None ? "none" : (net.auth == nm::Security::Enterprise ? "enterprise" : "unknown")))))},
                 {"encrypted", net.encrypted},
                 {"signal", net.signal}}));
        }
    }

    m_cmd.response(nlohmann::json({{"networks", nets},
                                   {"count", nets.size()}}),
                   Result::Ok);
    return true;
}

bool CommandExecutor::ConnectToWiFi()
{
    const nlohmann::json& networkInfo = m_cmd.request();

    auto ssid = networkInfo.find("ssid");
    auto password = networkInfo.find("password");
    if (ssid == networkInfo.end() || password == networkInfo.end() || !ssid->is_string() || !password->is_string())
    {
        m_cmd.response(Result::BadCommand);
        return true;
    }

    std::string ssidStr = ssid->get<std::string>();
    std::string passwordStr = password->get<std::string>();
    if (ssidStr.empty() || passwordStr.size() < 8)
    {
        m_cmd.response(Result::BadCommand);
        return true;
    }

    auto nm = Singleton<NetworkManager>::i();

    if (nm != nullptr)
    {
        for (const WifiNetwork &net : nm->networks())
        {
            if (net.ssid == ssidStr)
            {
                auto uuid = nm->connect(ssidStr, passwordStr);
                m_cmd.response({{"uuid", uuid}}, Result::Ok);
                Singleton<AquaBox>::get()->doLater("activating_connection", [uuid, nm]() {
                    nm->activateConnection(uuid);
                });

                return true;
            }
        }
    }

    m_cmd.response(Result::NotFound);
    return true;
}