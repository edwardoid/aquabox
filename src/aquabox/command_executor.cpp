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
#include "relay.h"
#include "sensor.h"
#include "rule.h"
#include "update_events.h"
#include "network_manager.h"
#include <boost/asio/ip/host_name.hpp>

using namespace aquabox;

bool CommandExecutor::exec(const Command& cmd)
{
    m_cmd = cmd;
    bool ok = false;
    switch (cmd.operation())
    {
        case Operation::GetStatus: {
            ok = GetStatus();
            break;
        }
        case Operation::GetDevices: {
            ok = GetDevices();
            break;
        }
        case Operation::GetDevice: {
            ok = GetDevice();
            break;
        }
        case Operation::UpdateDevice: {
            ok = UpdateDevice();
            break;
        }
        case Operation::GetRules: {
            ok = GetRules();
            break;
        }
        case Operation::GetRule: {
            ok = GetRule();
            break;
        }
        case Operation::CreateRule: {
            ok = CreateOrUpdateRule(false);
            break;
        }
        case Operation::UpdateRule: {
            ok = CreateOrUpdateRule(true);
            break;
        }
        case Operation::DeleteRule: {
            ok = DeleteRule();
            break;
        }
        case Operation::DeviceRules: {
            ok = DeviceRules();
            break;
        }
        case Operation::SupportedRepeatTimeouts: {
            ok = SupportedRepeatTimeouts();
            break;
        }
        case Operation::TurnOn: {
            ok = TurnOn();
            break;
        }
        case Operation::TurnOff: {
            ok = TurnOff();
            break;
        }
        case Operation::ScanWiFiNetworks: {
            ok = ScanWiFiNetworks();
            break;
        }
        case Operation::WiFiNetworks: {
            ok = ListNetworks();
            break;
        }
        case Operation::ConnectToWiFi: {
            ok = ConnectToWiFi();
            break;
        }
        default: {
            LOG_ERROR << "Unsupported operation " << (int)(cmd.operation());
        }
    }
    const_cast<Command&>(cmd) = m_cmd;
    return ok;
}

bool CommandExecutor::GetStatus()
{
    nlohmann::json host({
        {"rest", Settings::integer(Settings::Keys::HTTP_PORT, Settings::Defaults::REST_PORT)},
        {"stream", Settings::integer(Settings::Keys::STREAM_PORT, Settings::Defaults::WS_PORT)},
        {"serial", Settings::string(Settings::Keys::SERIAL, Settings::Defaults::SERIAL )}
    });

    host["network"] = nlohmann::json({
        { "ssid", NetworkManager::i().currentNetwork.value.ssid },
        { "ip", NetworkManager::i().currentNetwork.value.ip },
        { "internet",  NetworkManager::i().connectedToInternet() }});

    host["host"] = boost::asio::ip::host_name();

    host["config"] = AquaBox::i().configurationData();

    m_cmd.response(nlohmann::json({{"aquabox", host}, {"status" , "online"}}), Result::Ok);
    return true;
}

bool CommandExecutor::GetDevices()
{
    std::vector<nlohmann::json> all;
    for (auto &dev : AquaBox::i().devices()) {
        nlohmann::json json;
        dev.second->json(json);
        if (dev.second->isRelay()) {
            json["isOn"] = std::dynamic_pointer_cast<Relay>(dev.second)->turnedOn();
        } else if(dev.second->isSensor()) {
            json["value"] = std::dynamic_pointer_cast<Sensor>(dev.second)->value();
        }
        all.push_back(json);
    }
    m_cmd.response(nlohmann::json({{ "devices", all }}), Result::Ok);
    return true;
}

bool CommandExecutor::GetDevice()
{
    if (!m_cmd.hasParameter(":dev")) {
        LOG_ERROR << "No device id parameter in URL";
        m_cmd.response(Result::BadCommand);
        return true;
    }
    
    std::string dev = m_cmd.getParameter(":dev");

    auto& devices = AquaBox::i().devices();
    auto pos = devices.find(dev);
    if (pos == devices.end()) {
        LOG_ERROR << "No device " << dev << " not found";
        m_cmd.response(Result::NotFound);
        return true;
    }

    nlohmann::json data;
    pos->second->json(data);
    if (pos->second->isRelay()) {
        data["isOn"] = std::dynamic_pointer_cast<Relay>(pos->second)->turnedOn();
    } else if(pos->second->isSensor()) {
        data["value"] = std::dynamic_pointer_cast<Sensor>(pos->second)->value();
    }

    m_cmd.response(data, Result::Ok);
    return true;
}

bool CommandExecutor::UpdateDevice()
{
    if (!m_cmd.hasParameter(":dev")) {
        LOG_ERROR << "No device id parameter in URL";
        m_cmd.response(Result::BadCommand);
        return true;
    }
    std::string dev = m_cmd.getParameter(":dev");
    
    auto& devices = AquaBox::i().devices();
    auto pos = devices.find(dev);
    if (pos == devices.end()) {
        LOG_ERROR << "No device " << dev << " found";
        m_cmd.response(Result::NotFound);
        return true;
    }

    nlohmann::json data;
    nlohmann::json deviceData;
    try {
        deviceData = nlohmann::json::parse(m_cmd.body());
    }
    catch (nlohmann::detail::parse_error &e) {
        data["data"] = m_cmd.body();
        data["error"] = e.what();
        data["cause"] = "Parse Error";
        m_cmd.response(data, Result::BadCommand);
        return true;
    }

    std::shared_ptr<Device> d = std::make_shared<Device>();
    if (!d->parse(deviceData)) {
        m_cmd.response(data, Result::BadCommand);
        return true;
    }

    auto& device = pos->second;
    nlohmann::json before;
    device->json(before);
    if (pos->second->isRelay()) {
        before["isOn"] = std::dynamic_pointer_cast<Relay>(pos->second)->turnedOn();
    }

    device->copyData(*d);

    AquaBox::i().updateDevice(dev);
    AquaBox::i().postUpdate(std::make_shared<DeviceUpdatedEvent>(device, before));

    m_cmd.response(Result::Ok);
}

bool CommandExecutor::GetRules()
{
    std::vector<nlohmann::json> all;
    for (auto &rule : AquaBox::i().rules()) {
        nlohmann::json data;
        rule.second->json(data);
        all.push_back(data);
    }
    nlohmann::json res = nlohmann::json::object();
    res["rules"] = all;
    m_cmd.response(nlohmann::json({{"rules", all}}), Result::Ok);
    return true;
}

bool CommandExecutor::GetRule()
{
    if (!m_cmd.hasParameter(":rule")) {
        LOG_ERROR << "No rule id parameter in URL";
        m_cmd.response(Result::BadCommand);
        return true;
    }
    std::string rule = m_cmd.getParameter(":rule");

    auto all = AquaBox::i().rules();
    auto pos = all.find(rule);

    if (pos == all.end()) {
        m_cmd.response(Result::NotFound);
    } else {
        nlohmann::json data;
        pos->second->json(data);
        m_cmd.response(data, Result::Ok);
    }
    return true;
}

bool CommandExecutor::CreateOrUpdateRule(bool update)
{
    if (!m_cmd.hasParameter(":dev")) {
        LOG_ERROR << "No device id parameter in URL";
        m_cmd.response(Result::BadCommand);
        return true;
    }
    std::string dev = m_cmd.getParameter(":dev");
    nlohmann::json data;
    nlohmann::json ruleData;
    try {
        ruleData = nlohmann::json::parse(m_cmd.body());
    } catch (nlohmann::detail::parse_error &e) {
        data["data"] = m_cmd.body();
        data["error"] = e.what();
        data["cause"] = "Parse Error";
        m_cmd.response(data, Result::BadCommand);
        return true;
    }

    auto r = std::make_shared<Rule>();
    try {
        if (!r->parse(ruleData)) {
            m_cmd.response(data, Result::BadCommand);
            return true;
        }
    } catch (nlohmann::detail::parse_error &e) {
        LOG_ERROR << "Rule can not be parsed because: " << e.what();
        m_cmd.response(data, Result::BadCommand);
        return true;
    }

    auto device = r->getDevice().resolve(); 

    if (device == nullptr) {
        data["data"] = m_cmd.body();
        data["error"] = "Can't find device";
        m_cmd.response(data, Result::BadCommand);
        return true;
    }

    if (!AquaBox::i().addRule(r, update)) {
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
    if (!m_cmd.hasParameter(":rule")) {
        LOG_ERROR << "No rule id parameter in URL";
        m_cmd.response(Result::BadCommand);
        return true;
    }
    auto rule = m_cmd.getParameter(":rule");

    auto& all = AquaBox::i().rules();
    auto pos = all.find(rule);

    if (pos == all.end()) {
        LOG_DEBUG << "Requested to delete unexisting rule " << rule;
        m_cmd.response(Result::NotFound);
        return true;
    }

    if (AquaBox::i().removeRule(pos->second)) {
        LOG_DEBUG << "Rule " << rule << " removed";
        m_cmd.response(Result::Ok);
    } else {
        LOG_ERROR << "Rule " << rule << " removal failed";
        m_cmd.response(Result::ServerError);
    }
    return true;
}

bool CommandExecutor::DeviceRules()
{
    if (!m_cmd.hasParameter(":dev")) {
        LOG_ERROR << "No dev id parameter in URL";
        m_cmd.response(Result::BadCommand);
        return true;
    }
    std::string dev = m_cmd.getParameter(":dev");
    std::vector<nlohmann::json> all;
    for (auto &rule : AquaBox::i().rules()) {
        if (((std::string)rule.second->getDevice().Value) == dev) {
            nlohmann::json data;
            rule.second->json(data);
            all.push_back(data);
        }
    }
    nlohmann::json res({{"rules", all},
                        {"device", dev}});
    m_cmd.response(res, Result::Ok);
    return true;
}

bool CommandExecutor::SupportedRepeatTimeouts()
{
    std::vector<RepeatUnit> units;
    for (auto u : Serializable<RepeatUnit>::val2Str) {
        units.push_back(u.first);
    }
    nlohmann::json res;
    res["units"] = Serializable<RepeatUnit>::val2Str;
    m_cmd.response(res, Result::Ok);
    return true;
}

bool CommandExecutor::TurnOn()
{
    if (!m_cmd.hasParameter(":dev")) {
        LOG_ERROR << "No dev id parameter in URL";
        m_cmd.response(Result::BadCommand);
        return true;
    }
    std::string dev = m_cmd.getParameter(":dev");
    auto pos = AquaBox::i().devices().find(dev);
    if (pos == AquaBox::i().devices().end()) {
        m_cmd.response(Result::NotFound);
        return true;
    }

    if (!pos->second->isRelay()) {
        m_cmd.response(Result::BadCommand);
        return true;
    }

    auto relay = std::dynamic_pointer_cast<Relay>(pos->second);

    relay->turnOn();
    nlohmann::json data;
    relay->json(data);
    data["isOn"] = relay->turnedOn();
    AquaBox::i().postUpdate(std::make_shared<ManualStateChangedEvent>(relay));

    m_cmd.response(data, Result::Ok);
    return true;
}

bool CommandExecutor::TurnOff()
{
    if (!m_cmd.hasParameter(":dev")) {
        LOG_ERROR << "No dev id parameter in URL";
        m_cmd.response(Result::BadCommand);
        return true;
    }
    std::string dev = m_cmd.getParameter(":dev");

    auto pos = AquaBox::i().devices().find(dev);
    if (pos == AquaBox::i().devices().end()) {
        m_cmd.response(Result::NotFound);
        return true;
    }
    
    if (!pos->second->isRelay()) {
        m_cmd.response(Result::BadCommand);
        return true;
    }

    auto relay = std::dynamic_pointer_cast<Relay>(pos->second);
    
    relay->turnOff();
    nlohmann::json data;
    relay->json(data);
    data["isOn"] = relay->turnedOn();

    AquaBox::i().postUpdate(std::make_shared<ManualStateChangedEvent>(relay));

    m_cmd.response(data, Result::Ok);
    return true;
}

bool CommandExecutor::ScanWiFiNetworks()
{
    bool res = NetworkManager::i().requestScan();
    m_cmd.response(nlohmann::json({{"requestResult", res}}), Result::Ok);
    return true;
}

bool CommandExecutor::ListNetworks()
{
    std::vector<nlohmann::json> nets;
    auto currentNetwork = NetworkManager::i().currentNetwork.value;
    for (auto& net : NetworkManager::i().networks())
    {
        nets.emplace_back(nlohmann::json(
            {
                { "ssid" , net.ssid },
                { "ip", ( net.ssid == currentNetwork.ssid ? currentNetwork.ip : "" ) },
                { "auth" , (net.auth == Authentication::WEP ? "wep" :
                           (net.auth == Authentication::WPA ? "wpa" :
                           (net.auth == Authentication::WPA2? "wpa2":
                           (net.auth == Authentication::None? "none": 
                           (net.auth == Authentication::Enterprise ? "enterprize" : "unknown"))))) },
                { "encrypted", net.encrypted },
                { "signal", net.signal }
            }
        ));
    }

    m_cmd.response(nlohmann::json({
        {"networks", nets},
        {"count", nets.size()}
    }), Result::Ok);
    return true;
}

bool CommandExecutor::ConnectToWiFi()
{
    nlohmann::json networkInfo;
    try {
        networkInfo = nlohmann::json::parse(m_cmd.body());
    }
    catch(...) {
        m_cmd.response(Result::BadCommand);
        return true;
    }

    auto ssid = networkInfo.find("ssid");
    auto password = networkInfo.find("password");
    if (ssid == networkInfo.end() || password == networkInfo.end() || !ssid->is_string() || !password->is_string()) {
        m_cmd.response(Result::BadCommand);
        return true;
    }

    WifiNetwork wifi;
    wifi.ssid = ssid->get<std::string>();
    wifi.password = password->get<std::string>();
    if(wifi.ssid.empty() || wifi.password.size() < 8) {
        m_cmd.response(Result::BadCommand);
        return true;
    }

    m_cmd.response(NetworkManager::i().connect(
        Settings::string(Settings::Keys::CLIENT_INTERFACE, Settings::Defaults::CLIENT_INTERFACE),
        wifi) ? Result::Ok : Result::BadCommand);
    return true;
}