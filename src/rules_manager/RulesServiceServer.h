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
#include <rules.grpc.pb.h>
#include <mutex>
#include <condition_variable>
#include <string>
#include <unordered_map>
#include <queue>
#include <thread>
#include "../micro_clients/settings.h"
#include "../rpc/server_base.h"

namespace aquabox
{
    namespace services
    {
        class RulesServiceServer final: public ServerBase<RulesService>
        {
        public:
            RulesServiceServer();
            virtual ~RulesServiceServer();
            virtual ::grpc::Status GetAll(::grpc::ServerContext* /*context*/, const ::google::protobuf::Empty* /*request*/, ::grpc::ServerWriter< ::aquabox::services::RuleData>* writer) override;
        private:
            void notifyConsumers();
        private:
        };
    }
}
