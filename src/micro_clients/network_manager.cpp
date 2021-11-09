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
    google::protobuf::Empty e;
    services::NetworkInfo data;

    if (client()->call(&services::NetworkManager::Stub::GetCurrentNetwork, e, &data).ok()) {
        info.uuid = data.uuid();
        info.ssid = data.ssid();
        info.strength = data.strength();
        info.secure = data.mode() == services::NetworkInfo::Secure;
    }

    return info;
}