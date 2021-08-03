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
#include <events.grpc.pb.h>
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
        class EventsBusServer final: public ServerBase<EventsBus>
        {
        public:
            EventsBusServer();
            virtual ~EventsBusServer();
            virtual ::grpc::Status Notify(::grpc::ServerContext* context, const ::aquabox::services::EventData* request, ::google::protobuf::Empty* response) override;
            virtual ::grpc::Status Subscribe(::grpc::ServerContext* context, const ::aquabox::services::ConsumerIdentifier* request, ::google::protobuf::Empty* response) override;
            virtual ::grpc::Status Unsubscribe(::grpc::ServerContext* context, const ::aquabox::services::ConsumerIdentifier* request, ::google::protobuf::Empty* response) override;
            virtual ::grpc::Status GetUpdates(::grpc::ServerContext* context, const ::aquabox::services::ConsumerIdentifier* request, ::grpc::ServerWriter< ::aquabox::services::EventData>* writer) override;
        private:
            void notifyConsumers();
            std::string getId(::grpc::ServerContext*) const;
        private:
            std::mutex m_mx;
            
            std::queue<aquabox::services::EventData> m_pendingEvents;
            std::unordered_map<std::string, std::queue<::aquabox::services::EventData>> m_events;
            std::mutex m_newEnevtsAvailableMx;
            std::condition_variable m_newEnevtsAvailableCv;
            std::thread m_consumersNotifier;
        };
    }
}
