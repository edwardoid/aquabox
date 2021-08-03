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

#include <iostream>
#include <grpcpp/server_builder.h>
#include <rules.grpc.pb.h>
#include "RulesServiceServer.h"

using namespace aquabox;

int main()
{
    services::RulesServiceServer service;

    std::string server_address("0.0.0.0:");

    {
        server_address += std::to_string(Singleton<ISettings>::i()->port(services::RulesService::service_full_name(), true));
    }
  
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server " << services::RulesService::service_full_name() << " listening on " << server_address << std::endl;
    server->Wait();
    return 0;
}