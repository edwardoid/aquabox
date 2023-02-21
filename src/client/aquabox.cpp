/*
 * AquaBox
 * Copyright (C) 2019-2023  Eduard Sargsyan
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

#include "aquabox.h"
#include "cloud_stream.h"
#include "global_timer.h"
#include "lock_file.h"
#include "options.h"
#include "rest_api.h"
#include "web_server.h"
#include <chrono>
#include <common/log.h>
#include <common/singleton.h>
#include <exception>
#include <micro_clients/events.h>
#include <micro_clients/network_manager.h>
#include <micro_clients/settings.h>
#include <pstream.h>
#include <random>
#include <types/network_event.h>
#include <types/rule.h>
#include <types/types.h>
#include <types/update_events.h>

#define LOG_FILE_SIZE_IN_MB 10
#define BYTES_IN_MB 1000000
#define MAX_LOG_FILES 50

using namespace aquabox;

AquaBox::AquaBox()
    : Application("client")
    , m_pool(std::thread::hardware_concurrency())
    , m_server(new WebServer)
{
    // PerformanceMetrics::update();
}

AquaBox::~AquaBox()
{
    delete m_server;
}

int AquaBox::start()
{
    Singleton<Cloud>::available = !optionsIsSet("standalone");

    const std::string severity = Singleton<ISettings>::i()->get(Settings::Keys::LOG_LEVEL,
                                                                std::string(plog::severityToString(plog::Severity::info)));

    plog::init(plog::severityFromString(severity.c_str()), &m_consoleAppender);

    m_postUpdatesThread = std::move(std::thread([this]() { while(1) { this->postUpdate(m_pendingEvents.pop());  } }));

    srand(time(NULL));

    Singleton<IEvents>::i()->subscribe("rules_manager", [this](const std::shared_ptr<Event> e) {
        scheduleUpdate(e);
    });

    Singleton<IEvents>::i()->subscribe("devices_manager", [this](const std::shared_ptr<Event> e) {
        scheduleUpdate(e);
    });

    m_globalTimer = std::make_unique<GlobalTimer>();

    m_server->start();

    m_globalTimer->waitToFinish();

    m_postUpdatesThread.join();

    return EXIT_SUCCESS;
}

UsersMap& AquaBox::users() const
{
    return m_users;
}

void AquaBox::doLater(const std::string& name, std::function<void(void)>&& task)
{
    auto id = m_perfomance.taskStarted(name);
    boost::asio::post(m_pool, [this, id, t = std::move(task)] {
        t();
        m_perfomance.taskFinished(id);
    });
}

void AquaBox::scheduleUpdate(std::shared_ptr<Event> event)
{
    if(!m_pendingEvents.push(event))
    {
        LOG_WARNING << "Dropped an event because of too many unsent events are waiting to be sent.";
    }
}

void AquaBox::postUpdate(std::shared_ptr<Event> event)
{
    m_server->postUpdate(event);

    auto c = Singleton<Cloud>::i();

    if (c != nullptr) {
        c->postUpdate(event);
    }
}

std::string AquaBox::configurationData()
{
    std::string configurationDataString;
    configurationDataString += std::to_string(Singleton<ISettings>::i()->get(Settings::Keys::HTTP_PORT, Settings::Defaults::REST_PORT));
    configurationDataString += ";";

    configurationDataString += std::to_string(Singleton<ISettings>::i()->get(Settings::Keys::STREAM_PORT, Settings::Defaults::WS_PORT));
    configurationDataString += ";";

    configurationDataString += serial();
    configurationDataString += ";";

    if (Singleton<INetworkManager>::i() != nullptr)
    {
        std::string ip = Singleton<INetworkManager>::i()->currentNetwork().ip;
        if (ip.empty())
        {
            ip = boost::asio::ip::host_name();
        }
        configurationDataString += ip;
    }
    else
        configurationDataString += "aquabox";
    configurationDataString += ";";

    configurationDataString += std::to_string(16);

    
    return configurationDataString;
}

double AquaBox::systemTemperature() const
{
    double temp = 0;
#ifdef TARGET_DEVICE
    redi::ipstream tempCmd("cat /sys/devices/virtual/thermal/thermal_zone0/temp");
    tempCmd >> temp;
    temp /= 1000.;
#else
    temp = 65.89;
#endif
    return temp;
}
