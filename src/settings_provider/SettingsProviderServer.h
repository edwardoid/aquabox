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
#include <settings.grpc.pb.h>
#include <mutex>
#include <string>
#include <unordered_map>
#include "../rpc/server_base.h"

namespace aquabox
{
    namespace services
    {
        class SettingsProviderServer final: public ServerBase<SettingsProvider>
        {
        public:
            SettingsProviderServer(std::uint32_t startFrom);
            virtual ~SettingsProviderServer();
            virtual ::grpc::Status AllocatePort(::grpc::ServerContext* context, const ::google::protobuf::StringValue* request, ::google::protobuf::UInt32Value* response) override;
            virtual ::grpc::Status GetPort(::grpc::ServerContext* context, const ::google::protobuf::StringValue* request, ::google::protobuf::UInt32Value* response) override;
        private:
            std::mutex m_mx;
            std::unordered_map<std::string, std::uint32_t> m_ports;
            std::uint32_t m_lastPort = 50050;
        };
    }
}
