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

#include <unordered_map>
#include <mutex>
#include "results_map.h"
#include "command.h"

namespace aquabox
{
    class BoxesManager
    {
    public:
        static std::shared_ptr<class Box> get(const std::string& id, bool createIfNotExists = false);
        static bool eraseIfNeed(std::string id);
    private:
        static BoxesManager self;

        ResultsMap<Command> m_results;
        std::unordered_map<std::string, std::shared_ptr<class Box>> m_connections;
        std::mutex m_mx;
    };
}
