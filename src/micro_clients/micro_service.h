/*
 * AquaBox
 * Copyright (C) 2022  Eduard Sargsyan
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

#include <common/log.h>
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

namespace aquabox
{
    namespace services
    {
        template<class MicroServiceInterface, int timeout = 500>
        class MicroService
        {
        public:
            MicroService(std::uint32_t port)
                : MicroService(std::string("127.0.0.1:") + std::to_string(port))
            {}
            MicroService(std::string address)
                : MicroService(grpc::CreateChannel(address, grpc::InsecureChannelCredentials()))
            {}
            MicroService(std::shared_ptr<grpc::Channel> channel)
                : iface(MicroServiceInterface ::NewStub(channel))
            {}
            std::shared_ptr<class MicroServiceInterface::Stub> operator->() { return iface; }
            template<typename Function, typename... Args>
            grpc::Status call(Function fn, Args... args)
            {
                auto p = iface.get();
                grpc::ClientContext cxt;
                if (timeout != 0)
                {
                    cxt.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(timeout));
                }
                return (p->*fn)(&cxt, args...);
            }
            virtual ~MicroService() {}

        protected:
            std::shared_ptr<class MicroServiceInterface::Stub> iface;
        };
    } // namespace services
} // namespace aquabox
