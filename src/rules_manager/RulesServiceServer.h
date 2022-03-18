/*
 * AquaBox
 * Copyright (C) 2019-2022  Eduard Sargsyan
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
#include <rpc/server_base.h>
#include <rules.grpc.pb.h>

namespace aquabox
{
    class Rule;
    namespace services
    {
        class RulesServiceServer final : public ServerBase<RulesService>
        {
        public:
            RulesServiceServer();
            virtual ~RulesServiceServer();
            virtual bool init() override;
            virtual ::grpc::Status Add(::grpc::ServerContext* context,
                                       const ::aquabox::services::RuleData* request,
                                       ::google::protobuf::BoolValue* response) override;
            virtual ::grpc::Status Update(::grpc::ServerContext* context,
                                          const ::aquabox::services::RuleData* request,
                                          ::google::protobuf::BoolValue* response) override;
            virtual ::grpc::Status Remove(::grpc::ServerContext* context,
                                          const ::google::protobuf::StringValue* request,
                                          ::google::protobuf::BoolValue* response) override;
            virtual ::grpc::Status GetAll(::grpc::ServerContext* /*context*/,
                                          const ::google::protobuf::Empty* /*request*/,
                                          ::grpc::ServerWriter<::aquabox::services::RuleData>* writer) override;
            virtual ::grpc::Status Get(::grpc::ServerContext* context,
                                       const ::google::protobuf::StringValue* request,
                                       ::aquabox::services::RuleData* response) override;

        private:
        };
    } // namespace services
} // namespace aquabox
