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

#include "box_connection.h"
#include <s2s.h>
#include <set>

namespace aquabox
{
    class ApplicationConnection;

    class Box : public BoxConnection
    {
    public:
        Box(const std::string& id, ResultsMap<Command>& results);
        SignalSlot::Signal<const nlohmann::json&> UpdateReceived;
        void addConnection(std::shared_ptr<ApplicationConnection> connection);
        void removeConnection(std::shared_ptr<ApplicationConnection> connection);
        std::vector<std::shared_ptr<ApplicationConnection>> connections() const;
        std::set<int64_t> telegramChats() const;

    private:
        std::vector<std::shared_ptr<ApplicationConnection>> m_applications;
    };
} // namespace aquabox
