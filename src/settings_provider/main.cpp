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

#include "SettingsProviderServer.h"
#include <grpcpp/server_builder.h>
#include <services/application.h>

class SettingsApp: public aquabox::Application
{
public:
    SettingsApp(): aquabox::Application("settings") {}
    virtual int start() override
    {
        aquabox::services::SettingsProviderServer service(500500);

        std::string server_address("0.0.0.0:500500");

        grpc::ServerBuilder builder;
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
        LOG_INFO << "Server " << service.service_full_name() << " listening on " << server_address;
        server->Wait();
        return EXIT_SUCCESS;
    }
};

int main(int argc, char** argv)
{
    SettingsApp app;
    return app.exec(argc, argv);
}