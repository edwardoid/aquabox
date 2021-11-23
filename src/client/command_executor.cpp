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
#include <types/rule.h>
#include <types/types.h>
#include "aquabox.h"
#include "cloud.h"
#include <types/update_events.h>
#include <micro_clients/settings.h>
#include <micro_clients/devices_manager.h>
#include <micro_clients/rules_manager.h>
#include <micro_clients/network_manager.h>
#include <boost/asio/ip/host_name.hpp>

using namespace aquabox;

bool CommandExecutor::exec(const Command& cmd)
{
    m_cmd = cmd;
    bool ok = false;
    switch (cmd.operation()) {
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
    case Operation::Set: {
        ok = SetValue();
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
    nlohmann::json host(
        { { "rest", Singleton<ISettings>::i()->get(Settings::Keys::HTTP_PORT, Settings::Defaults::REST_PORT) },
            { "stream", Singleton<ISettings>::i()->get(Settings::Keys::STREAM_PORT, Settings::Defaults::WS_PORT) },
            { "serial", SERIAL_NUMBER } });

    auto network = Singleton<INetworkManager>::i()->currentNetwork();
    host["network"] = nlohmann::json({ { "ssid", network.ssid },
        { "ip", network.ip },
        { "internet", Singleton<INetworkManager>::i()->hasInternetConnection() } });

    host["host"] = boost::asio::ip::host_name();

    host["config"] = Singleton<AquaBox>::get()->configurationData();

    auto cloud = Singleton<Cloud>::i();
    bool isRegistered = cloud == nullptr ? false : (bool)cloud->isRegistered;

    m_cmd.response(nlohmann::json({ { "aquabox", host },
                       { "status", "online" },
                       { "registered", isRegistered } }),
        Result::Ok);
    return true;
}

bool CommandExecutor::GetDevices()
{
    std::vector<nlohmann::json> all;
    for(auto& dev : Singleton<IDevicesManager>::i()->all()) {
        nlohmann::json json = dev;

        for(auto& p : Singleton<IDevicesManager>::i()->properties(dev.id())) {
            json[p] = Singleton<IDevicesManager>::i()->value(dev.id(), p);
        }
        all.push_back(json);
    };

    m_cmd.response(nlohmann::json({ { "devices", all } }), Result::Ok);
    return true;
}

bool CommandExecutor::GetDevice()
{
    if (!m_cmd.hasParameter(":dev")) {
        LOG_ERROR << "No device id parameter in URL";
        m_cmd.response(Result::BadCommand);
        return true;
    }

    std::string id = m_cmd.getParameter(":dev");

    auto dev = Singleton<IDevicesManager>::i()->get(id);
    if (dev.id().empty()) {
        LOG_ERROR << "No device " << id << " not found";
        m_cmd.response(Result::NotFound);
        return true;
    }

    nlohmann::json data = dev;
    for(auto& p : Singleton<IDevicesManager>::i()->properties(id)) {
        data[p] = Singleton<IDevicesManager>::i()->value(id, p);
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

    std::string id = m_cmd.getParameter(":dev");

    DeviceRef d;
    try {
        d = m_cmd.request();
    } catch (nlohmann::detail::parse_error& e) {
        nlohmann::json data;
        data["request"] = m_cmd.request();
        data["error"] = e.what();
        data["cause"] = "Parse Error";
        m_cmd.response(data, Result::BadCommand);
        return true;
    }

    auto src = Singleton<IDevicesManager>::i()->get(id);

    if(src.id().empty()) {
        LOG_ERROR << "No device " << id << " not found";
        m_cmd.response(Result::NotFound);
        return true;
    }

    nlohmann::json before = src;

    for(auto& p : Singleton<IDevicesManager>::i()->properties(id)) {
        before[p] = Singleton<IDevicesManager>::i()->value(id, p);
    }

    if(Singleton<IDevicesManager>::i()->update(d)) {
        m_cmd.response(Result::Ok);
    } else {
        m_cmd.response(Result::ServerError);
    }

    return true;
}

bool CommandExecutor::GetRules()
{
    std::vector<nlohmann::json> all;
    for(const Rule& rule : Singleton<IRulesManager>::i()->all())
    {
        nlohmann::json data = rule;
        all.push_back(data);
    };
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
    std::string id = m_cmd.getParameter(":rule");

    Rule rule = Singleton<IRulesManager>::i()->get(id);

    if (rule.id() != id) {
        m_cmd.response(Result::NotFound);
    } else {
        nlohmann::json data = rule;
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
    Rule r;
    try {
        r = m_cmd.request();
    } catch (nlohmann::detail::parse_error& e) {
        LOG_ERROR << "Rule can not be parsed because: " << e.what();
        m_cmd.response(e.what(), Result::BadCommand);
        return true;
    }

    bool res = update ? (Singleton<IRulesManager>::i()->update(r)) : (Singleton<IRulesManager>::i()->add(r));

    if (!res) {
        m_cmd.response("Wrong method or internal data", Result::BadCommand);
        return true;
    }

    m_cmd.response(m_cmd.request(), Result::Ok);
    return true;
}

bool CommandExecutor::DeleteRule()
{
    if (!m_cmd.hasParameter(":rule")) {
        LOG_ERROR << "No rule id parameter in URL";
        m_cmd.response(Result::BadCommand);
        return true;
    }
    auto id = m_cmd.getParameter(":rule");
    if (Singleton<IRulesManager>::i()->remove(id)) {
        LOG_DEBUG << "Rule " << id << " removed";
        m_cmd.response(Result::Ok);
    } else {
        LOG_ERROR << "Rule " << id << " removal failed";
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

    std::vector<Rule> allRules = Singleton<IRulesManager>::i()->all();

    std::for_each(allRules.cbegin(), allRules.cend(), [&all, dev](const Rule& rule) {
        if (rule.device() == dev) {
            nlohmann::json data = rule;
            all.push_back(data);
        }
    });

    nlohmann::json res({ { "rules", all }, { "device", dev } });
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
    if (!m_cmd.hasParameter(":dev")) {
        LOG_ERROR << "No dev id parameter in URL";
        m_cmd.response(Result::BadCommand);
        return true;
    }

    auto changes = m_cmd.request()["changes"];
    if (!changes.is_array()) {
        LOG_ERROR << "Set value should be array";
    }

    std::string id = m_cmd.getParameter(":dev");
    auto dev = Singleton<IDevicesManager>::i()->get(id);
    if (dev.id().empty()) {
        m_cmd.response(Result::NotFound);
        return true;
    }

    nlohmann::json data = dev;
    for (auto v : changes) {
        std::string property = v["property"].get<std::string>();

        if(Singleton<IDevicesManager>::i()->set(id, property, v["value"])) {
            data[property] = Singleton<IDevicesManager>::i()->value(id, property);
            LOG_INFO << "Updated value for " << property << " to be " << data[property].dump();
        } else {
            LOG_ERROR << "Can't set " << property << " to be " << v["value"].dump();
        }
    }

    Singleton<IDevicesManager>::i()->update(dev);

    m_cmd.response(data, Result::Ok);
    return true;
}

bool CommandExecutor::ScanWiFiNetworks()
{
    bool res = Singleton<INetworkManager>::i()->startScan();

    m_cmd.response(nlohmann::json({ { "requestResult", res } }), Result::Ok);
    return true;
}

bool CommandExecutor::ListNetworks()
{
    std::vector<nlohmann::json> nets;

    for (auto& net : Singleton<INetworkManager>::i()->availableNetworks()) {
        nets.emplace_back(nlohmann::json(
            { { "ssid", net.ssid },
                { "ip", net.ip },
                { "auth", net.auth },
                { "encrypted", net.secure },
                { "signal", net.strength } }));
    }

    m_cmd.response(nlohmann::json({ { "networks", nets }, { "count", nets.size() } }),
        Result::Ok);
    return true;
}

bool CommandExecutor::ConnectToWiFi()
{
    const nlohmann::json& networkInfo = m_cmd.request();

    auto ssid = networkInfo.find("ssid");
    auto password = networkInfo.find("password");
    if (ssid == networkInfo.end() || password == networkInfo.end() || !ssid->is_string() || !password->is_string()) {
        m_cmd.response(Result::BadCommand);
        return true;
    }

    std::string ssidStr = ssid->get<std::string>();
    std::string passwordStr = password->get<std::string>();
    if (ssidStr.empty() || passwordStr.size() < 8) {
        m_cmd.response(Result::BadCommand);
        return true;
    }

    std::string uuid = Singleton<INetworkManager>::i()->connect(ssidStr, passwordStr);

    if(uuid.empty())
        m_cmd.response(Result::NotFound);
    else
        m_cmd.response({ { "uuid", uuid } }, Result::Ok);
    return true;
}