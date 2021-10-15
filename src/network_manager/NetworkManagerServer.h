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

#pragma once
#include <mutex>
#include <network.grpc.pb.h>

namespace nm {
class NetworkManager;
}

namespace aquabox {
namespace services {
    class NetworkManagerServer final : public NetworkManager::Service {
    public:
        NetworkManagerServer();
        virtual ~NetworkManagerServer();
        virtual ::grpc::Status GetCurrentNetwork(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::aquabox::services::NetworkInfo* response) override;
        virtual ::grpc::Status IsConnectedToInternet(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::google::protobuf::BoolValue* response) override;
        virtual ::grpc::Status Connect(::grpc::ServerContext* context, const ::aquabox::services::NetworkConnect* request, ::grpc::ServerWriter<::aquabox::services::ConnectionStatus>* writer) override;
        virtual ::grpc::Status StartHotspot(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::google::protobuf::Empty* response) override;
        virtual ::grpc::Status StopHotspot(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::google::protobuf::Empty* response) override;
        virtual ::grpc::Status CurrentNetworkChanged(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::grpc::ServerWriter<::aquabox::services::NetworkInfo>* writer) override;

    private:
        std::mutex m_lock;
        std::unique_ptr<nm::NetworkManager> m_nm;
    };
}
}
