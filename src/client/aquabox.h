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

#include "serial.h"
#include "path.h"
#include "log.h"
#include "lockablemap.h"
#include "performance_metrics.h"
#include <boost/asio.hpp>
#include <memory>
#include <singleton.h>

#define LOG_FILE_SIZE_IN_MB 10
#define BYTES_IN_MB 1000000
#define MAX_LOG_FILES 50

namespace aquabox
{
    class Device;
    class Rule;
    class User;
    class Event;
    class WebServer;
    class Timer;
    class RulesTimer;
    class ThreadPool;
    class GlobalTimer;

    using DevicesMap = LockableHashMap<std::string, std::shared_ptr<Device>>;
    using RulesMap = LockableHashMap<std::string, std::shared_ptr<Rule>>;
    using UsersMap = LockableHashMap<std::string, std::shared_ptr<User>>;

    class AquaBox
    {
    protected:
        AquaBox(const AquaBox&) = delete;
        AquaBox(AquaBox&&) = delete;
        AquaBox& operator = (const AquaBox&) = delete;
        AquaBox();
        friend class Singleton<AquaBox>;
    public:
        int start();
        ~AquaBox();
        std::shared_ptr<Device> findDevice(const std::string& id);
        std::shared_ptr<Rule> findRule(const std::string& id);
        const Path& workingDirectory() const;
        DevicesMap& devices() const;
        UsersMap& users() const;
        RulesMap& rules() const;
        bool addRule(std::shared_ptr<Rule>& rule, bool replace = false);
        bool removeRule(const std::string& id);
        bool updateDevice(std::string id);
        void doLater(const std::string& name, std::function<void(void)>&& task);
        void postUpdate(std::shared_ptr<Event> event);
        bool assistantIsConnected() const { return false; }
        double systemTemperature() const;
        std::string configurationData() const;
    private:
        void onCurrentNetworkChanged(const class CurrentNetworkState& state);
        void trackSensors();
        void readDevices(const Path& root);
        void readUsers();
        void readRules(const Path& root);
    private:
        PerformanceMetrics m_perfomance;
        boost::asio::thread_pool m_pool;
        plog::ColorConsoleAppender<plog::TxtFormatter> m_consoleAppender;
        plog::RollingFileAppender<plog::TxtFormatter> m_fileAppender;
        mutable DevicesMap m_devices;
        mutable UsersMap m_users;
        mutable RulesMap m_rules;
        WebServer* m_server;
        Path m_wd;
        Path m_devicesDir;
        Path m_rulesDir;
        std::unique_ptr<GlobalTimer> m_globalTimer;
        RulesTimer* m_rulesTimer = nullptr;
    };
}
