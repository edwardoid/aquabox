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

#include "cloud_stream.h"
#include <common/lockablemap.h>
#include <atomic>
#include <client_http.hpp>
#include <memory>
#include <s2s_property.h>
#include <common/timeout.h>
#include <unordered_map>

using WsClient = SimpleWeb::SocketClient<SimpleWeb::WS>;
using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;

namespace aquabox {
class Cloud {
public:
    Cloud();
    void connect();
    void disconnect();

    SignalSlot::Property<bool> isRegistered;
    bool checkIfRegistered();
    void doRegister();
    void heartheat();
    void postUpdate(std::shared_ptr<class Event> event);
    std::shared_ptr<CloudStream> stream(std::string name);

private:
    bool requestStreams();
    bool isConnected();

private:
    std::atomic_bool m_working;
    LockableHashMap<std::string, std::shared_ptr<CloudStream>> m_streams;
    Timeout m_updateTimeout;
};
}
