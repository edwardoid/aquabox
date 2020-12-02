/*
 * AquaBox
 * Copyright (C) 2020  Eduard Sargsyan
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

#include <string>
#include <s2s/s2s_property.h>
#include <memory>

namespace aquabox
{
    using ClusterPtr = std::shared_ptr<Cluster>;
    using AbstractDevicePtr = std::shared_ptr<AbstractDevice>;

    class Identifiable
    {
        std::string id;
    };

    class AbstractDevice: public Identifiable
    {
        std::string name;
        std::string type; 
        std::string address; // "cluster/id"
        ClusterPtr cluster;
        uint32_t capabilities;
        SignalSlot::Property<std::string> value;
    };

    class Cluster: public Identifiable
    {
        SignalSlot::Property<bool> online;
        std::vector<AbstractDevicePtr> devices();
    };
}