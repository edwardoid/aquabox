/*
 * AquaBox
 * Copyright (C) 2022  Eduard Sargsyan
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

#include "events_queue.h"
#include "log.h"
#include <micro_clients/settings.h>

template<>
aquabox::EventsQueue* aquabox::Singleton<aquabox::EventsQueue>::create()
{
    EventsQueue* q = new EventsQueue();
    static std::thread t([q]() {
        try {
            const int port = Settings::integer(Settings::Keys::MQTT_PORT, Settings::Defaults::MQTT_PORT);
            q->start(port);
        }
        catch (boost::wrapexcept<boost::system::system_error> e) {
            LOG_ERROR << e.what();
        }
    });
    return q;
}

template<>
void aquabox::Singleton<aquabox::EventsQueue>::destroy()
{
    if (instance == nullptr) {
        instance->stop();
        delete instance;
        instance = nullptr;
    }
}