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
#include <nlohmann/json.hpp>
#include <rpc/server_base.h>
#include <settings.grpc.pb.h>
#include <string>
#include <unordered_map>

namespace aquabox
{
    namespace services
    {
        class SettingsProviderServer final : public ServerBase<SettingsProvider>
        {
        public:
            SettingsProviderServer(std::uint32_t startFrom);
            virtual ~SettingsProviderServer();
            virtual ::grpc::Status Has(::grpc::ServerContext* context,
                                       const ::google::protobuf::StringValue* request,
                                       ::google::protobuf::BoolValue* response) override;
            virtual ::grpc::Status AllocatePort(::grpc::ServerContext* context,
                                                const ::google::protobuf::StringValue* request,
                                                ::google::protobuf::UInt32Value* response) override;
            virtual ::grpc::Status GetPort(::grpc::ServerContext* context,
                                           const ::google::protobuf::StringValue* request,
                                           ::google::protobuf::UInt32Value* response) override;
            virtual ::grpc::Status GetString(::grpc::ServerContext* context,
                                             const ::google::protobuf::StringValue* request,
                                             ::google::protobuf::StringValue* response) override;
            virtual ::grpc::Status SetString(::grpc::ServerContext* context,
                                             const ::aquabox::services::SettingsSetStringRequest* request,
                                             ::google::protobuf::Empty* response) override;

            virtual ::grpc::Status GetBool(::grpc::ServerContext* context,
                                           const ::google::protobuf::StringValue* request,
                                           ::google::protobuf::BoolValue* response) override;
            virtual ::grpc::Status SetBool(::grpc::ServerContext* context,
                                           const ::aquabox::services::SettingsSetBoolRequest* request,
                                           ::google::protobuf::Empty* response) override;

            virtual ::grpc::Status GetInt(::grpc::ServerContext* context,
                                           const ::google::protobuf::StringValue* request,
                                           ::google::protobuf::Int32Value* response) override;
            virtual ::grpc::Status SetInt(::grpc::ServerContext* context,
                                           const ::aquabox::services::SettingsSetIntRequest* request,
                                           ::google::protobuf::Empty* response) override;

        private:
            void load();
            void save();

        private:
            std::recursive_mutex m_mx;
            std::string m_json_path;
            std::unordered_map<std::string, std::uint32_t> m_ports;
            nlohmann::json m_data;
            std::uint32_t m_lastPort = 50050;
        };
    } // namespace services
} // namespace aquabox
