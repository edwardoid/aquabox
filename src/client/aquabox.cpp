/*
 * AquaBox
 * Copyright (C) 2019-2020  Eduard Sargsyan
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
#include <types/types.h>
#include <types/rule.h>
#include "cloud.h"
#include <types/network_events.h>
#include <types/update_events.h>
#include "global_timer.h"
#include "lock_file.h"
#include <common/log.h>
#include "options.h"
#include "rest_api.h"
#include <common/singleton.h>
#include <micro_clients/settings.h>
#include <micro_clients/events.h>
#include <micro_clients/network_manager.h>
#include "web_server.h"
#include <chrono>
#include <exception>
#include <pstream.h>
#include <random>

#define LOG_FILE_SIZE_IN_MB 10
#define BYTES_IN_MB 1000000
#define MAX_LOG_FILES 50

using namespace aquabox;

AquaBox::AquaBox()
    : Application("client")
    , m_pool(Settings::integer(Settings::Keys::THREADS,
        std::thread::hardware_concurrency()))
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

    plog::init(plog::severityFromString(
                   Settings::string(Settings::Keys::LOG_LEVEL,
                       plog::severityToString(plog::Severity::info))
                       .c_str()),
        &m_consoleAppender);

    LOG_INFO << "Starting " << Settings::Defaults::APPLICATION << " ver. "
             << std::hex << Settings::Defaults::VERSION_MAJOR << "."
             << Settings::Defaults::VERSION_MINOR;

    srand(time(NULL));
    std::string procPath = "/proc/";
    procPath += std::to_string(static_cast<int>(getpid()));
    procPath += "/exe";

    char exePath[1024];
    memset(exePath, '\0', 1024);

    int bytes = std::min<size_t>(readlink(procPath.c_str(), exePath, 1024), 1024 - 1);
    if (bytes >= 0) {
        exePath[bytes] = '\0';
    }

    std::string cwd = exePath;

    auto pos = cwd.find_last_of('/');
    if (pos > 0)
        cwd.erase(pos);

    Singleton<IEvents>::i()->subscribe("rules_manager", [this](const std::shared_ptr<Event> e) {
        LOG_ERROR << e->serialize().dump();
        postUpdate(e);
    });

    Singleton<IEvents>::i()->subscribe("devices_manager", [this](const std::shared_ptr<Event> e) {
        LOG_ERROR << e->serialize().dump();
        postUpdate(e);
    });

    m_globalTimer = std::make_unique<GlobalTimer>();

    m_server->start();

    m_globalTimer->waitToFinish();

    return EXIT_SUCCESS;
}

UsersMap& AquaBox::users() const { return m_users; }

void AquaBox::doLater(const std::string& name,
    std::function<void(void)>&& task)
{
    auto id = m_perfomance.taskStarted(name);
    boost::asio::post(m_pool, [this, id, t = std::move(task)] {
        t();
        m_perfomance.taskFinished(id);
    });
}

void AquaBox::postUpdate(std::shared_ptr<Event> event)
{
    m_server->postUpdate(event);

    auto c = Singleton<Cloud>::i();

    if (c != nullptr) {
        c->postUpdate(event);
    }
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

std::string AquaBox::configurationData() const
{
    std::string configurationDataString;
    configurationDataString += std::to_string(Settings::integer(
        Settings::Keys::HTTP_PORT, Settings::Defaults::REST_PORT));
    configurationDataString += ";";

    configurationDataString += std::to_string(Settings::integer(
        Settings::Keys::STREAM_PORT, Settings::Defaults::WS_PORT));
    configurationDataString += ";";

    configurationDataString += SERIAL_NUMBER;
    configurationDataString += ";";


    configurationDataString += Singleton<INetworkManager>::i()->currentNetwork().ip;
    configurationDataString += ";";

    configurationDataString += std::to_string((int)Singleton<AquaBox>::get()->systemTemperature());
    return configurationDataString;
}
