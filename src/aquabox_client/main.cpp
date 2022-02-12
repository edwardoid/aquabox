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

#include "AquaboxClientServer.h"
#include <common/singleton.h>
#include <grpcpp/server_builder.h>
#include <micro_clients/settings.h>
#include <services/rpc_application.h>

int main(int argc, const char** argv)
{
    aquabox::RPCApplication<aquabox::services::AquaboxClientServer> app("ab_client");
    return app.exec(argc, argv);
}
