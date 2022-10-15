/*
 * AquaBox
 * Copyright (C) 2019-2022  Eduard Sargsyan
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

#include "boxes_manager.h"
#include "box.h"
#include "database.h"

using namespace aquabox;

BoxesManager BoxesManager::self;

std::shared_ptr<Box> BoxesManager::get(const std::string& id, bool createIfNotExists)
{
    std::lock_guard<std::mutex> ready(self.m_mx);

    auto p = self.m_connections.find(id);
    auto box = p == self.m_connections.end() ? nullptr : p->second;

    if (box == nullptr && Database::i().wasProduced(id)) {
        box = std::make_shared<Box>(id, self.m_results);
        self.m_connections.insert({id, box});
    }

    return box;
}

bool BoxesManager::eraseIfNeed(std::string id)
{
    std::lock_guard<std::mutex> ready(self.m_mx);
    auto p = self.m_connections.find(id);
    if (p != self.m_connections.end() && p->second->empty()) {
        self.m_connections.erase(p);
        return true;
    }

    return false;
}
