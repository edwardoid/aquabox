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

#include <common/log.h>
#include <common/singleton.h>
#include <grpcpp/server_builder.h>
#include <memory>
#include <micro_clients/settings.h>
#include <services/application.h>

namespace aquabox
{
    template<typename Implementation>
    class RPCApplication : public Application
    {
    public:
        RPCApplication(const std::string& identifier)
            : Application(identifier)
        {}
        virtual int start() override
        {
            m_impl = std::make_unique<Implementation>();

            std::string server_address("0.0.0.0:");

            {
                server_address += std::to_string(Singleton<ISettings>::i()->port(Implementation::service_full_name(), true));
            }

            const std::string severity = Singleton<ISettings>::i()->get(Settings::Keys::LOG_LEVEL,
                                                                        std::string(plog::severityToString(plog::Severity::info)));
            plog::get()->setMaxSeverity(plog::severityFromString(severity.c_str()));

            grpc::ServerBuilder builder;
            builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
            builder.RegisterService(m_impl.get());
            std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
            LOG_INFO << "Server " << Implementation::service_full_name() << " listening on " << server_address;
            server->Wait();
            return 0;
        }

    protected:
        std::unique_ptr<Implementation> m_impl;
    };
} // namespace aquabox