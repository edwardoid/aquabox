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

#include "rpc_event.h"
#include "service.h"
#include <functional>
#include <mutex>
#include <queue>
#include <unordered_map>

namespace aquabox {
namespace services {
    class RPCEventService : public Service<class RPCEventService> {
    public:
        RPCEventService() = default;

        virtual ~RPCEventService()
        {
        }

        void notify(const std::string& sender, int instance, const std::string& receipent, int receipentInstance, const std::string& event);
        void notify(const std::string& sender, const std::string& event);
        void on(const std::string& sender, std::string event, std::function<void(const std::string)> cb);

    private:
        std::string id(const std::string sender, const std::string& event) const;
        void dispatch(const RPCEvent& ev);

    private:
        std::unordered_multimap<std::string, std::function<void(const std::string)>> m_callbacks;
        std::queue<RPCEvent> m_queue;
    };
}
}