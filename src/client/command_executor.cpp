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
    case Operation::TurnOn:
    {
        ok = TurnOn();
        break;
    }
    case Operation::TurnOff:
    {
        ok = TurnOff();
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
    Singleton<AquaBox>::get()->devices().for_each([this, &all](auto key, auto dev) {
        nlohmann::json json = *dev;
        if (dev->supports(DeviceClass::Relay))
        {
            json["isOn"] = std::dynamic_pointer_cast<AbstractRelay>(dev)->turnedOn();
        }
        if (dev->supports(DeviceClass::Sensor))
        {
            json["value"] = std::dynamic_pointer_cast<AbstractSensor>(dev)->value();
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

    auto dev = Singleton<AquaBox>::get()->devices().get(id, nullptr);
    if (dev == nullptr)
    {
        LOG_ERROR << "No device " << dev << " not found";
        m_cmd.response(Result::NotFound);
        return true;
    }

    nlohmann::json data = *dev;
    if (dev->supports(DeviceClass::Relay))
    {
        data["isOn"] = std::dynamic_pointer_cast<AbstractRelay>(dev)->turnedOn();
    }
    if (dev->supports(DeviceClass::Sensor))
    {
        data["value"] = std::dynamic_pointer_cast<AbstractSensor>(dev)->value();
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

    nlohmann::json data;
    std::shared_ptr<Device> d = std::make_shared<Device>();
    try
    {
        *d = nlohmann::json::parse(m_cmd.body());
    }
    catch (nlohmann::detail::parse_error &e)
    {
        data["data"] = m_cmd.body();
        data["error"] = e.what();
        data["cause"] = "Parse Error";
        m_cmd.response(data, Result::BadCommand);
        return true;
    }

    bool found = Singleton<AquaBox>::get()->devices().on(id, [this, d](auto id, auto dev) {
        nlohmann::json before = *dev;
        if (dev->supports(DeviceClass::Relay))
        {
            before["isOn"] = std::dynamic_pointer_cast<AbstractRelay>(dev)->turnedOn();
        }

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
    nlohmann::json data;
    nlohmann::json ruleData;
    try
    {
        ruleData = nlohmann::json::parse(m_cmd.body());
    }
    catch (nlohmann::detail::parse_error &e)
    {
        data["data"] = m_cmd.body();
        data["error"] = e.what();
        data["cause"] = "Parse Error";
        m_cmd.response(data, Result::BadCommand);
        return true;
    }

    auto r = std::make_shared<Rule>();
    try
    {
        *r = ruleData;
    }
    catch (nlohmann::detail::parse_error &e)
    {
        LOG_ERROR << "Rule can not be parsed because: " << e.what();
        m_cmd.response(data, Result::BadCommand);
        return true;
    }

    if (r->resolveDevice() == nullptr)
    {
        data["data"] = m_cmd.body();
        data["error"] = "Can't find device";
        m_cmd.response(data, Result::BadCommand);
        return true;
    }

    if (!Singleton<AquaBox>::get()->addRule(r, update))
    {
        data["cause"] = "Wrong method or internal data";
        m_cmd.response(data, Result::BadCommand);
        return true;
    }

    data["rule"] = ruleData;
    data["cause"] = "ok";
    m_cmd.response(data, Result::Ok);
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

bool CommandExecutor::TurnOn()
{
    if (!m_cmd.hasParameter(":dev"))
    {
        LOG_ERROR << "No dev id parameter in URL";
        m_cmd.response(Result::BadCommand);
        return true;
    }
    std::string id = m_cmd.getParameter(":dev");
    auto dev = Singleton<AquaBox>::get()->devices().get(id, nullptr);
    if (dev == nullptr)
    {
        m_cmd.response(Result::NotFound);
        return true;
    }

    if (!dev->supports(DeviceClass::Relay))
    {
        m_cmd.response(Result::BadCommand);
        return true;
    }

    auto relay = std::dynamic_pointer_cast<AbstractRelay>(dev);

    relay->turnOn();
    nlohmann::json data; 
    data = *relay;
    data["isOn"] = relay->turnedOn();
    Singleton<AquaBox>::get()->postUpdate(std::make_shared<ManualStateChangedEvent>(relay));

    m_cmd.response(data, Result::Ok);
    return true;
}

bool CommandExecutor::TurnOff()
{
    if (!m_cmd.hasParameter(":dev"))
    {
        LOG_ERROR << "No dev id parameter in URL";
        m_cmd.response(Result::BadCommand);
        return true;
    }
    std::string id = m_cmd.getParameter(":dev");
    auto dev = Singleton<AquaBox>::get()->devices().get(id, nullptr);
    if (dev == nullptr)
    {
        m_cmd.response(Result::NotFound);
        return true;
    }

    if (!dev->supports(DeviceClass::Relay))
    {
        LOG_ERROR << id << " is not relay";
        m_cmd.response(Result::BadCommand);
        return true;
    }

    auto relay = std::dynamic_pointer_cast<AbstractRelay>(dev);

    relay->turnOff();
    nlohmann::json data = *relay;
    data["isOn"] = relay->turnedOn();

    Singleton<AquaBox>::get()->postUpdate(std::make_shared<ManualStateChangedEvent>(relay));

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
    nlohmann::json networkInfo;
    try
    {
        networkInfo = nlohmann::json::parse(m_cmd.body());
    }
    catch (...)
    {
        m_cmd.response(Result::BadCommand);
        return true;
    }

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