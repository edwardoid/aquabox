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
#include "cloud_stream.h"
#include <boost/asio/ip/host_name.hpp>
#include <micro_clients/devices_manager.h>
#include <micro_clients/network_manager.h>
#include <micro_clients/rules_manager.h>
#include <micro_clients/settings.h>
#include <types/rule.h>
#include <types/types.h>
#include <types/update_events.h>

namespace aquabox
{
    void to_json(nlohmann::json& j, const ExecutionResult& res)
    {
        to_json(j, (const BaseStreamResponse&) res);
        if (res)
        {
            to_json(j, res.ok);
        }
        else
        {
            to_json(j, res.fail);
        }
    }
}

using namespace aquabox;

ExecutionResult CommandExecutor::exec(const CommandRequest& cmd)
{
    m_res.rpid = cmd.rqid;
    m_res.ok.setOperation(cmd.operation());
    m_res.ok.setParameters(cmd.parameters());
    m_res.ok.rpid = cmd.rqid;
    m_res.fail.rpid = cmd.rqid;
    m_cmd = cmd;
    Result ok = Result::Ok;
    switch (cmd.operation()) {
    case Operation::SubscribeToBox: {
        ok = Subscribe();
        break;
    }
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
        m_res = ErrorResponse::unsupportedCommand();
        LOG_ERROR << "Unsupported operation " << (int) (cmd.operation());
    }
    }

    m_res.ok.setParameters(m_cmd.parameters());
    m_res.fail.setResult(ok);

    if (m_res)
        m_res.type = m_res.ok.type;
    else
        m_res.type = m_res.fail.type;
    return m_res;
}

Result CommandExecutor::Subscribe()
{
    nlohmann::json host({{"rest", Singleton<ISettings>::i()->get(Settings::Keys::HTTP_PORT, Settings::Defaults::REST_PORT)},
                         {"stream", Singleton<ISettings>::i()->get(Settings::Keys::STREAM_PORT, Settings::Defaults::WS_PORT)},
                         {"serial", serial() }});

    auto network = Singleton<INetworkManager>::i()->currentNetwork();
    host["network"] = nlohmann::json(
        {{"ssid", network.ssid}, {"ip", network.ip}, {"internet", Singleton<INetworkManager>::i()->hasInternetConnection()}});

    host["host"] = boost::asio::ip::host_name();

    host["config"] = Singleton<AquaBox>::get()->configurationData();

    auto cloud = Singleton<Cloud>::i();
    bool isRegistered = cloud == nullptr ? false : (bool) cloud->isRegistered();

    const nlohmann::json data({{"aquabox", host}, {"status", "online"}, {"registered", isRegistered}});
    Singleton<AquaBox>::i()->scheduleUpdate(std::make_shared<Event>(Event::cameOnline(serial())));
    m_res.ok.setData(data);
    return Result::Ok;
}

Result CommandExecutor::GetStatus()
{
    nlohmann::json host({{"rest", Singleton<ISettings>::i()->get(Settings::Keys::HTTP_PORT, Settings::Defaults::REST_PORT)},
                         {"stream", Singleton<ISettings>::i()->get(Settings::Keys::STREAM_PORT, Settings::Defaults::WS_PORT)},
                         {"serial", serial()}});

    auto network = Singleton<INetworkManager>::i()->currentNetwork();
    host["network"] = nlohmann::json(
        {{"ssid", network.ssid}, {"ip", network.ip}, {"internet", Singleton<INetworkManager>::i()->hasInternetConnection()}});

    host["host"] = boost::asio::ip::host_name();

    host["config"] = Singleton<AquaBox>::get()->configurationData();

    auto cloud = Singleton<Cloud>::i();
    bool isRegistered = cloud == nullptr ? false : (bool) cloud->isRegistered();

    const nlohmann::json data({{"aquabox", host}, {"status", "online"}, {"registered", isRegistered}});
    
    m_res.ok.setData(data);
    return Result::Ok;
}

Result CommandExecutor::GetDevices()
{
    std::vector<nlohmann::json> all;
    for (auto& dev : Singleton<IDevicesManager>::i()->all()) {
        nlohmann::json json = dev;

        for (auto& p : Singleton<IDevicesManager>::i()->properties(dev.id())) {
            json[p] = Singleton<IDevicesManager>::i()->value(dev.id(), p);
        }
        all.push_back(json);
    };

    m_res.ok.setData({{"devices", all}});
    return Result::Ok;
}

Result CommandExecutor::GetDevice()
{
    if (!m_cmd.hasParameter(":dev")) {
        LOG_ERROR << "No device id parameter in URL";
        m_res.fail = ErrorResponse::missedParameter(":dev");
        return m_res.fail.result();
    }

    std::string id = m_cmd.getParameter(":dev");

    auto dev = Singleton<IDevicesManager>::i()->get(id);
    if (dev.id().empty()) {
        m_res.fail = ErrorResponse::deviceNotFound(id);
        return m_res.fail.result();
    }

    nlohmann::json data = dev;
    for (auto& p : Singleton<IDevicesManager>::i()->properties(id)) {
        data[p] = Singleton<IDevicesManager>::i()->value(id, p);
    }

    m_res.ok.setData(data);
    return Result::Ok;
}

Result CommandExecutor::UpdateDevice()
{
    if (!m_cmd.hasParameter(":dev")) {
        LOG_ERROR << "No device id parameter in URL";
        m_res.fail = ErrorResponse::missedParameter(":dev");
        return m_res.fail.result();
    }

    std::string id = m_cmd.getParameter(":dev");

    DeviceRef d;
    try {
        d = m_cmd.data();
    }
    catch (nlohmann::detail::parse_error& e) {
        m_res.fail = ErrorResponse::parseError();
        return m_res.fail.result();
    }

    auto src = Singleton<IDevicesManager>::i()->get(id);

    if (src.id().empty()) {
        LOG_ERROR << "No device " << id << " not found";
        m_res.fail = ErrorResponse::deviceNotFound(id);
        return m_res.fail.result();
    }

    nlohmann::json before = src;

    for (auto& p : Singleton<IDevicesManager>::i()->properties(id)) {
        before[p] = Singleton<IDevicesManager>::i()->value(id, p);
    }

    if (Singleton<IDevicesManager>::i()->update(d)) {
        m_res.ok.setData(d);
        return Result::Ok;
    }

    return Result::ServerError;
}

Result CommandExecutor::GetRules()
{
    std::vector<nlohmann::json> all;
    for (const Rule& rule : Singleton<IRulesManager>::i()->all()) {
        nlohmann::json data = rule;
        all.push_back(data);
    };
    m_res.ok.setData(nlohmann::json({{"rules", all}}));
    return Result::Ok;
}

Result CommandExecutor::GetRule()
{
    if (!m_cmd.hasParameter(":rule")) {
        LOG_ERROR << "No rule id parameter in URL";
        m_res.fail = ErrorResponse::missedParameter(":rule");
        return Result::BadCommand;
    }
    std::string id = m_cmd.getParameter(":rule");

    Rule rule = Singleton<IRulesManager>::i()->get(id);

    if (rule.id() != id) {
        m_res.fail = ErrorResponse::ruleNotFound(id);
        return m_res.fail.result();
    }

    m_res.ok.setData(as_json(rule));
    return Result::Ok;
}

Result CommandExecutor::CreateOrUpdateRule(bool update)
{
    if (!m_cmd.hasParameter(":dev")) {
        LOG_ERROR << "No device id parameter in URL";
        m_res.fail = ErrorResponse::missedParameter(":dev");
        return m_res.fail.result();
    }
    std::string dev = m_cmd.getParameter(":dev");
    Rule r;
    try {
        r = m_cmd.data();
    }
    catch (nlohmann::detail::parse_error& e) {
        LOG_ERROR << "Rule can not be parsed because: " << e.what();
        m_res.fail = ErrorResponse::parseError();
        m_res.fail.setDetails(e.what());
        return m_res.fail.result();
    }

    bool res = update ? (Singleton<IRulesManager>::i()->update(r)) : (Singleton<IRulesManager>::i()->add(r));

    if (!res) {
        m_res.fail = ErrorResponse(Result::BadCommand, "Wrong method or internal data");
        return m_res.fail.result();
    }

    m_res.ok.setData(m_cmd.data());
    return Result::Ok;
}

Result CommandExecutor::DeleteRule()
{
    if (!m_cmd.hasParameter(":rule")) {
        LOG_ERROR << "No rule id parameter in URL";
        m_res.fail = ErrorResponse::missedParameter(":rule");
        return m_res.fail.result();
    }
    auto id = m_cmd.getParameter(":rule");
    if (Singleton<IRulesManager>::i()->remove(id)) {
        LOG_DEBUG << "Rule " << id << " removed";
    }
    else {
        LOG_ERROR << "Rule " << id << " removal failed";
        m_res.fail = ErrorResponse(Result::ServerError, "Can't remove rule");
        return m_res.fail.result();
    }
    return Result::Ok;
}

Result CommandExecutor::DeviceRules()
{
    if (!m_cmd.hasParameter(":dev")) {
        LOG_ERROR << "No dev id parameter in URL";
        m_res.fail = ErrorResponse::missedParameter(":dev");
        return m_res.fail.result();
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

    nlohmann::json data({{"rules", all}, {"device", dev}});
    m_res.ok.setData(data);
    return Result::Ok;
}

Result CommandExecutor::SetValue()
{
    if (!m_cmd.hasParameter(":dev")) {
        LOG_ERROR << "No dev id parameter in URL";
        m_res.fail = ErrorResponse::missedParameter(":dev");
        return m_res.fail.result();
    }

    auto changes = m_cmd.data()["changes"];
    if (!changes.is_array()) {
        LOG_ERROR << "Set value should be array";
        m_res.fail = ErrorResponse::wrongType("changes", "array");
        return m_res.fail.result();
    }

    std::string id = m_cmd.getParameter(":dev");
    auto dev = Singleton<IDevicesManager>::i()->get(id);
    if (dev.id().empty()) {
        m_res.fail = ErrorResponse::deviceNotFound(id);
        return m_res.fail.result();
    }

    nlohmann::json data = dev;
    for (auto v : changes) {
        std::string property = v["property"].get<std::string>();

        if (Singleton<IDevicesManager>::i()->set(id, property, v["value"])) {
            data[property] = Singleton<IDevicesManager>::i()->value(id, property);
            LOG_INFO << "Updated value for " << property << " to be " << data[property].dump();
        }
        else {
            LOG_ERROR << "Can't set " << property << " to be " << v["value"].dump();
        }
    }

    Singleton<IDevicesManager>::i()->update(dev);

    return Result::Ok;
}

Result CommandExecutor::ScanWiFiNetworks()
{
    bool res = Singleton<INetworkManager>::i()->startScan();

    m_res.ok.setData(nlohmann::json({{"requestResult", res}}));
    return Result::Ok;
}

Result CommandExecutor::ListNetworks()
{
    std::vector<nlohmann::json> nets;

    for (auto& net : Singleton<INetworkManager>::i()->availableNetworks()) {
        nets.emplace_back(
            nlohmann::json({{"ssid", net.ssid}, {"ip", net.ip}, {"auth", net.auth}, {"encrypted", net.secure}, {"signal", net.strength}}));
    }

    m_res.ok.setData(nlohmann::json({{"networks", nets}, {"count", nets.size()}}));
    return Result::Ok;
}

Result CommandExecutor::ConnectToWiFi()
{
    const nlohmann::json& networkInfo = m_cmd.data();

    auto ssid = networkInfo.find("ssid");
    
    auto password = networkInfo.find("password");

    if (ssid == networkInfo.end() || password == networkInfo.end() || !ssid->is_string() || !password->is_string())
    {
        m_res.fail = ErrorResponse::parseError();
        return m_res.fail.result();
    }

    std::string ssidStr = ssid->get<std::string>();
    std::string passwordStr = password->get<std::string>();
    if (ssidStr.empty()) {
        m_res.fail = ErrorResponse::wrongType("SSID", "string");
        return m_res.fail.result();
    }
    
    if(passwordStr.size() < 8) {
        m_res.fail = ErrorResponse::wrongType("password", "string");
        return m_res.fail.result();
    }

    std::string uuid = Singleton<INetworkManager>::i()->connect(ssidStr, passwordStr);

    if (uuid.empty())
    {
        m_res.fail = ErrorResponse(Result::NotFound, "Wifi network not found", ssidStr);
        return m_res.fail.result();
    }

    m_res.ok.setData({{"uuid", uuid}});
    return Result::Ok;
}