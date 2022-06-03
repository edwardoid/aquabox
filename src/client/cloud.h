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

#pragma once

#include "cloud_stream.h"
#include <client_http.hpp>
#include <common/timeout.h>
#include <memory>
#include <s2s_property.h>
#include <unordered_map>

using WsClient = SimpleWeb::SocketClient<SimpleWeb::WS>;
using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;

namespace aquabox
{
    class Cloud
    {
    public:
        Cloud();
        void connect();

        SignalSlot::Property<bool> isRegistered;
        bool checkIfRegistered();
        void heartheat(bool force);
        bool postUpdate(std::shared_ptr<class Event> event);
        std::shared_ptr<CloudStream> stream(std::string name);

    private:
        bool connectToStreams();
        std::vector<std::string> requiredStreamNames();

    private:
        std::mutex m_mx;
        std::unordered_map<std::string, std::shared_ptr<CloudStream>> m_streams;
        Timeout m_updateTimeout;
    };
} // namespace aquabox
