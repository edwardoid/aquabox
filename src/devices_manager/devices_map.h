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

#include <common/lockablemap.h>
#include <memory>
#include <common/path.h>

namespace aquabox {
class DevicesMap : public LockableHashMap<std::string, std::shared_ptr<class Device>> {
public:
    DevicesMap();
    std::set<std::string> ids() const;
    bool hasDevice(const std::string& address) const;
    std::string deviceIdByAddress(const std::string& address) const;
    std::string nextId() const;
    std::string addDevice(std::shared_ptr<class Device> device, bool pending = false);
    bool parse(Path& root);
};
}
