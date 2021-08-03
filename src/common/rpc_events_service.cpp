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

#include "rpc_events_service.h"

using namespace aquabox;
using namespace aquabox::services;

void RPCEventService::notify(const std::string& sender, const std::string& event)
{
    notify(sender, 0, "*", 0, event);
}

void RPCEventService::notify(const std::string& sender, int instance, const std::string& receipent, int receipentInstance, const std::string& event)
{

}

void RPCEventService::on(const std::string& sender, std::string event, std::function<void(std::string)> cb)
{
    
    m_callbacks.insert({ id(sender, event), cb });
}

std::string RPCEventService::id(const std::string sender, const std::string& event) const
{
    std::string idStr = sender;
    idStr += "->";
    idStr += event;
    return idStr;
}

void RPCEventService::dispatch(const RPCEvent& ev)
{
    std::string idStr = id(ev.sender, ev.event);
    auto r = m_callbacks.equal_range(idStr);
    for(auto i = r.first; i != r.second; ++i)
    {
        i->second(ev.event);
    }
}