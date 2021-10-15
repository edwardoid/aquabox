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
#include <sstream>

using namespace aquabox;
using namespace aquabox::services;

std::string RPCEvent::to_string() const
{
    std::stringstream ss;
    ss << sender << '\n';
    ss << instance << '\n';
    ss << recipient << '\n';
    ss << recipientInstance << '\n';
    ss << event;
    return ss.str();
}

bool RPCEvent::from_string(const std::string& serialized)
{
    try {
        std::stringstream ss;
        ss << serialized;
        std::string field;
        if (std::getline(ss, field)) {
            sender = field;
        }
        if (std::getline(ss, field)) {
            instance = stoi(field);
        }
        if (std::getline(ss, field)) {
            recipient = field;
        }
        if (std::getline(ss, field)) {
            recipientInstance = stoi(field);
        }
        if (std::getline(ss, field)) {
            event = field;
        }
    } catch (...) {
        return false;
    }
    return true;
}
