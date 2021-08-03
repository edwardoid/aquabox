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

#include <aquabox.grpc.pb.h>

#include <mutex>
#include <string>
#include <unordered_map>
#include "../rpc/server_base.h"
#include "../micro_clients/micro_service.h"

namespace aquabox
{
    namespace services
    {
        class RulesService;
        class DevicesService;
        class NetworkManager;

        class AquaboxClientServer final: public ServerBase<AquaboxAPI>
        {
        public:
            AquaboxClientServer();
            virtual ~AquaboxClientServer();
            virtual ::grpc::Status devices(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::grpc::ServerWriter<DeviceData>* writer) override;
            virtual ::grpc::Status rules(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::grpc::ServerWriter<RuleData>* writer) override;
            virtual ::grpc::Status device(::grpc::ServerContext* context, const ::google::protobuf::StringValue* id, DeviceData* response) override;
            virtual ::grpc::Status authorize(::grpc::ServerContext* context, const ::google::protobuf::StringValue* request, ::google::protobuf::BoolValue* response) override;
            virtual ::grpc::Status rule(::grpc::ServerContext* context, const ::google::protobuf::StringValue* id, RuleData* response) override;
            virtual ::grpc::Status addRule(::grpc::ServerContext* context, const RuleData* request, ::google::protobuf::BoolValue* response) override;
            virtual ::grpc::Status updateRule(::grpc::ServerContext* context, const RuleData* request, ::google::protobuf::BoolValue* response) override;
            virtual ::grpc::Status updateDevice(::grpc::ServerContext* context, const DeviceData* request, ::google::protobuf::BoolValue* response) override;
            virtual ::grpc::Status scan(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::google::protobuf::BoolValue* response) override;
            virtual ::grpc::Status networks(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::grpc::ServerWriter<NetworkInfo>* writer) override;
            virtual ::grpc::Status connect(::grpc::ServerContext* context, const NetworkConnect* request, ::google::protobuf::BoolValue* response) override;
            virtual ::grpc::Status status(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, StatusInfo* response) override;
        private:
            std::unique_ptr<services::MicroService<services::RulesService>> m_rules;
            std::unique_ptr<services::MicroService<services::DevicesService>> m_devices;
            std::unique_ptr<services::MicroService<services::NetworkManager>> m_network;
        };
    }
}
