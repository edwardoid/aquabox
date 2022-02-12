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
#include <devices.grpc.pb.h>
#include <rpc/server_base.h>
#include <thread>
#include <common/thread_pool.h>

namespace aquabox
{
    namespace services
    {
        class DevicesServiceServer final : public ServerBase<DevicesService>
        {
        public:
            DevicesServiceServer();
            virtual ~DevicesServiceServer();

            virtual bool init() override;

            virtual ::grpc::Status GetAll(::grpc::ServerContext* context,
                                          const ::google::protobuf::Empty* request,
                                          ::grpc::ServerWriter<::aquabox::services::DeviceData>* writer) override;
            virtual ::grpc::Status Update(::grpc::ServerContext* context,
                                          const ::aquabox::services::DeviceData* request,
                                          ::google::protobuf::BoolValue* response) override;
            virtual ::grpc::Status Get(::grpc::ServerContext* context,
                                       const ::google::protobuf::StringValue* request,
                                       ::aquabox::services::DeviceData* response) override;
            virtual ::grpc::Status Set(::grpc::ServerContext* context,
                                       const ::aquabox::services::SetValueRequest* request,
                                       ::google::protobuf::BoolValue* response) override;
            virtual ::grpc::Status FetchProperties(::grpc::ServerContext* context,
                                                   const ::google::protobuf::StringValue* request,
                                                   ::grpc::ServerWriter<::google::protobuf::StringValue>* writer) override;
            virtual ::grpc::Status PropertyValue(::grpc::ServerContext* context,
                                                 const ::aquabox::services::GetPropertyValueRequest* request,
                                                 ::google::protobuf::DoubleValue* response) override;

        private:
            void processSensorUpdates();

        private:
            std::thread m_sensorPollingThread;
            aquabox::ThreadPool m_pool;
        };
    } // namespace services
} // namespace aquabox
