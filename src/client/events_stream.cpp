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

#include "events_stream.h"
#include <common/log.h>

using namespace aquabox;

EventsStream::EventsStream(const std::string& url)
    : CloudStream(url)
{
}

EventsStream::~EventsStream() { }

nlohmann::json EventsStream::messageReceived(const nlohmann::json& message)
{
    LOG_ERROR << "Events stream should not receive messages!";
    return message;
}