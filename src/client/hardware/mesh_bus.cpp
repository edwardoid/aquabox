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

#include "mesh_bus.h"
#include "../aquabox.h"
#include "hermes_incubator.h"
#include "hermes_hardware.h"
#include "hermes_mesh_gateway.h"
#include <settings.h>
#include <log.h>
#include <functional>

using namespace aquabox;

MeshBus::MeshBus()
{
    Singleton<HermesMeshGateway>::i()->newConnection.connect(std::bind(&MeshBus::newConnection, this, std::placeholders::_1));
}

void MeshBus::loop()
{
    Singleton<HermesMeshGateway>::i()->run();
}
