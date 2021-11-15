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

#pragma once

#include <common/lockablemap.h>
#include <common/log.h>
#include <common/performance_metrics.h>
#include <types/serial.h>
#include <boost/asio/thread_pool.hpp>
#include <memory>
#include <common/singleton.h>
#include <services/application.h>

#define LOG_FILE_SIZE_IN_MB 10
#define BYTES_IN_MB 1000000
#define MAX_LOG_FILES 50

namespace aquabox {
class Device;
class Rule;
class User;
class Event;
class WebServer;
class Timer;
class RulesTimer;
class ThreadPool;
class GlobalTimer;

using UsersMap = LockableHashMap<std::string, std::shared_ptr<User>>;

class AquaBox: public Application {
protected:
    AquaBox(const AquaBox&) = delete;
    AquaBox(AquaBox&&) = delete;
    AquaBox& operator=(const AquaBox&) = delete;
    AquaBox();
    friend class Singleton<AquaBox>;

public:
    int start();
    ~AquaBox();
    UsersMap& users() const;
    void doLater(const std::string& name, std::function<void(void)>&& task);
    void postUpdate(std::shared_ptr<Event> event);
    bool assistantIsConnected() const { return false; }
    double systemTemperature() const;
    std::string configurationData() const;

private:
    void onCurrentNetworkChanged(const class CurrentNetworkState& state);

private:
    PerformanceMetrics m_perfomance;
    boost::asio::thread_pool m_pool;
    plog::ColorConsoleAppender<plog::TxtFormatter> m_consoleAppender;
    mutable UsersMap m_users;
    WebServer* m_server;
    std::unique_ptr<GlobalTimer> m_globalTimer;
};
}
