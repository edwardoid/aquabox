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

#ifndef AQUABOX_H
#define AQUABOX_H

#include "serial.h"
#include <thread_pool.hpp>
#include "rules_timer.h"
#include "path.h"
#include <memory>
#include <unordered_map>

#define LOG_FILE_SIZE_IN_MB 10
#define BYTES_IN_MB 1000000
#define MAX_LOG_FILES 50

namespace aquabox
{
    class Device;
    class Rule;
    class User;
    class Event;
    class Cloud;
    class WebServer;
    class ScreenManager;
    class Timer;
    class RulesTimer;

    using DevicesMap = std::unordered_map<std::string, std::shared_ptr<Device>>;
    using RulesMap = std::unordered_map<std::string, std::shared_ptr<Rule>>;
    using UsersMap = std::unordered_map<std::string, std::shared_ptr<User>>;

    class AquaBox
    {
        AquaBox(const AquaBox&) = delete;
        AquaBox(AquaBox&&) = delete;
        AquaBox& operator = (const AquaBox&) = delete;
        AquaBox();
        static AquaBox Instance;
    public:
        static AquaBox& i();
        int start();
        ~AquaBox();
        std::shared_ptr<Device> findDevice(std::string id);
        std::shared_ptr<Rule> findRule(std::string id);
        const Path& workingDirectory() const;
        DevicesMap& devices() const;
        UsersMap& users() const;
        RulesMap& rules() const;
        bool addRule(std::shared_ptr<Rule>& rule, bool replace = false);
        bool removeRule(std::shared_ptr<Rule> rule);
        bool updateDevice(std::string id);
        void doLater(std::function<void(void)>&& task);
        void postUpdate(std::shared_ptr<Event> event);
        Cloud& cloud();
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
        plog::ColorConsoleAppender<plog::TxtFormatter> m_consoleAppender;
        plog::RollingFileAppender<plog::TxtFormatter> m_fileAppender;
        tp::ThreadPoolOptions m_poolOptions;
        std::unique_ptr<tp::ThreadPool> m_pool;
        mutable DevicesMap m_devices;
        mutable UsersMap m_users;
        mutable RulesMap m_rules;
        Cloud* m_cloud;
        WebServer* m_server;
        Path m_wd;
        Path m_devicesDir;
        Path m_rulesDir;
        class ScreenManager* m_screen = nullptr;
        RulesTimer* m_rulesTimer = nullptr;
        Timer* m_globalTimer = nullptr;
    };
}

#endif // AQUABOX_H