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

#include "network_manager.h"
#include "micro_service.h"
#include <services/service_identifier.h>
#include "settings.h"
#include <network.grpc.pb.h>

using namespace aquabox;

class NetworkManagerImpl : public INetworkManager {
public:
    NetworkManagerImpl();
    virtual ~NetworkManagerImpl() { }
    virtual NetworkState currentState() override;
    virtual NetworkInfo currentNetwork() override;
    virtual bool hasInternetConnection() override;
    virtual bool startScan(bool force = true) override;
    virtual std::vector<NetworkInfo> availableNetworks() override;
    virtual std::string connect(const std::string ssid, const std::string password) override;
};

namespace {
std::shared_ptr<services::MicroService<services::NetworkManager>> client()
{
    return std::make_shared<services::MicroService<services::NetworkManager>>(
        Singleton<ISettings>::i()->port(
            services::NetworkManager::service_full_name()));
}
} // namespace

template <>
INetworkManager* Singleton<INetworkManager>::create()
{
    return new NetworkManagerImpl();
}

NetworkManagerImpl::NetworkManagerImpl()
{}

NetworkState NetworkManagerImpl::currentState()
{
    google::protobuf::Empty e;
    services::ConnectionStatus data;

    if (client()->call(&services::NetworkManager::Stub::GetCurrentStatus, e, &data).ok()) {
        return static_cast<NetworkState>(data.status());
    }

    return NetworkState::Disconnected;
}

NetworkInfo NetworkManagerImpl::currentNetwork()
{
    NetworkInfo info;
    info.strength = -1;
    google::protobuf::Empty e;
    services::NetworkInfo data;

    LOG_DEBUG << "services::NetworkManager::Stub::GetCurrentNetwork";
    if (client()->call(&services::NetworkManager::Stub::GetCurrentNetwork, e, &data).ok()) {
        info.uuid = data.uuid();
        info.ssid = data.ssid();
        info.strength = data.strength();
        info.secure = data.mode() == services::NetworkInfo::Secure;
        info.ip = data.ip();
        info.auth = data.auth();
        LOG_INFO << "IP: " << data.ip();
    } else {
        LOG_ERROR << "services::NetworkManager::Stub::GetCurrentNetwork FAILED!";
    }

    return info;
}

bool NetworkManagerImpl::startScan(bool force)
{
    google::protobuf::BoolValue rq;
    google::protobuf::BoolValue rp;

    rq.set_value(force);

    if (client()->call(&services::NetworkManager::Stub::StartScan, rq, &rp).ok()) {
        return rp.value();
    }

    return false;
}

bool NetworkManagerImpl::hasInternetConnection()
{
    google::protobuf::Empty e;
    google::protobuf::BoolValue b;

    if (client()->call(&services::NetworkManager::Stub::IsConnectedToInternet, e, &b).ok()) {
        return b.value();
    }

    return false;
}

std::vector<NetworkInfo> NetworkManagerImpl::availableNetworks()
{
    grpc::ClientContext cxt;
    auto c = client();
    std::vector<NetworkInfo> res;
    google::protobuf::Empty e;
    std::unique_ptr<grpc::ClientReader<services::NetworkInfo>> reader(
        (*c)->GetAvailableNetworks(&cxt, e));
    services::NetworkInfo data;
    while (reader->Read(&data)) {
        NetworkInfo info;
        info.uuid = data.uuid();
        info.ssid = data.ssid();
        info.strength = data.strength();
        info.secure = data.mode() == services::NetworkInfo::Secure;
        info.ip = data.ip();
        info.auth = data.auth();
        res.push_back(static_cast<NetworkInfo>(info));
    }
    grpc::Status status = reader->Finish();
    return res;
}

std::string NetworkManagerImpl::connect(const std::string ssid, const std::string password)
{
    services::NetworkConnect c;
    c.set_ssid(ssid);
    c.set_password(password);
    services::ConnectionStatus u;

    if (client()
            ->call(&services::NetworkManager::Stub::Connect, c, &u)
            .ok()) {
        return u.uuid();
    }

    return "";
}