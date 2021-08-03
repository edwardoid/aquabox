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

#include "service.h"
#include <events.grpc.pb.h>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "../micro_clients/micro_service.h"


namespace aquabox
{
    class EventsBus: public Service<class EventsBus>
    {
    public:
        EventsBus(const std::string& identifier);
        virtual ~EventsBus();
        virtual bool interested(const std::string& sender);
        virtual void on(const std::string sender, std::function<bool (const services::EventData&)> callback);
        virtual void notify(std::string event);
    private:
        class Consumer final: public services::EventConsumer::Service
        {
        public:
            Consumer(EventsBus* self): m_self(self) {}
            virtual ~Consumer() {}
            virtual ::grpc::Status NewEventAvailable(::grpc::ServerContext* context, const services::EventDescriptor* request, ::google::protobuf::BoolValue* response) override
            {
                response->set_value(m_self->interested(request->sender()));
                return ::grpc::Status::OK;
            }
        private:
            EventsBus* m_self;
        };
        std::thread m_server;
        std::thread m_client;
        std::string m_identifier;
        std::mutex m_hasUpdateMx;
        std::condition_variable m_hasUpdateCv;
        bool m_hasPendingUpdates;
        std::unordered_multimap<std::string, std::function<bool (const services::EventData&)>> m_callbacks;
        services::MicroService<services::EventsBus> m_bus;
    };
}
