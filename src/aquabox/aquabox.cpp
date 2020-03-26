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

#include "aquabox.h"
#include "settings.h"
#include "lock_file.h"
#include "log.h"
#include "rest_api.h"
#include "rule.h"
#include "device.h"
#include "sensors_tracker.h"
#include "rule_parser.h"
#include "device_parser.h"
#include "user.h"
#include "update_events.h"
#include "screen_manager.h"
#include "options.h"
#include "repetive_task.h"
#include <exception>
#include <pstream.h>
#include <chrono>
#include <random>

#define LOG_FILE_SIZE_IN_MB 10
#define BYTES_IN_MB 1000000
#define MAX_LOG_FILES 50

using namespace aquabox;

AquaBox AquaBox::Instance;


AquaBox& AquaBox::i()
{
    return Instance;
}

AquaBox::AquaBox()
    : m_fileAppender("./../log/aquabox.log", LOG_FILE_SIZE_IN_MB * BYTES_IN_MB, MAX_LOG_FILES)
    , m_globalTimer("GlobalRoutines")
{}

int AquaBox::start()
{
    Lockfile lock;

    plog::init( plog::severityFromString(Settings::string(Settings::Keys::LOG_LEVEL, plog::severityToString(plog::Severity::info)).c_str()),
                &m_consoleAppender);

    if (!Options::Service) {
        plog::init( plog::severityFromString(Settings::string(Settings::Keys::LOG_LEVEL, plog::severityToString(plog::Severity::info)).c_str()),
                &m_fileAppender);
    }

    NetworkManager::i().setInterface(Settings::string(Settings::Keys::INTERFACE, Settings::Defaults::INTERFACE));
    NetworkManager::i().updateCurrentNetwork();

    LOG_INFO << "Starting " << Settings::Defaults::APPLICATION << " ver. " << std::hex << Settings::Defaults::VERSION_MAJOR << "." << Settings::Defaults::VERSION_MINOR;
    if(Options::Lock) {
        if (!lock.create()) {
            LOG_FATAL << "Another instance of " << Settings::Defaults::APPLICATION << " is running...";
            std::exit(EXIT_FAILURE);
        }
    }

    srand(time(NULL));
    std::string procPath = "/proc/";
    procPath += std::to_string(static_cast<int>(getpid()));
    procPath += "/exe";

    char cwd[1024];
    memset(cwd,'\0', 1024);

    int bytes = std::min<size_t>(readlink(procPath.c_str(), cwd, 1024), 1024 - 1);
    if(bytes >= 0) {
        cwd[bytes] = '\0';
    }

    m_wd = Settings::string(Settings::Keys::WORKING_DIRECTORY, cwd);
    if (!m_wd.normalize()) {
        LOG_ERROR << "Bad working directory path " << m_wd;
    } else {
        if(0 != chdir(m_wd.c_str())) {
            LOG_ERROR << "Can't change working directory to " << m_wd << " errno: " << errno << " : " << strerror(errno);
        } else {
            LOG_INFO << "Changed working direcotry to " << m_wd;
        }
    }

    m_poolOptions.setThreadCount(Settings::integer(Settings::Keys::THREADS, std::thread::hardware_concurrency()));
    m_poolOptions.setQueueSize(1024);

    m_pool.reset(new tp::ThreadPool(m_poolOptions));

    readDevices(m_wd);
    readRules(m_wd);
    readUsers();

    m_globalTimer.start([this](const Timer::ClockTimePoint&) {
        NetworkManager::i().updateCurrentNetwork();
        if (Options::Render) {
            m_screen.show();
        }
        if (Options::EnableCloud) {
            m_cloud.heartheat();
        }
        SensorsTracker::processUpdates();

    }, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::seconds(10)));

    m_timer.start();
    m_server.start();

    std::getchar();

    return EXIT_SUCCESS;
}

void AquaBox::readDevices(const Path& p)
{
    m_devicesDir = p;
    m_devicesDir.append("devices");
    m_devices = DeviceParser::parse(m_devicesDir);
}

void AquaBox::readUsers()
{
    auto users = Settings::array(Settings::Keys::USERS);

    LOG_INFO << "Loading " << users.size() << " users...";
    for (auto u : users) {
        auto usr = std::make_shared<User>();
        if(usr->init(u)) {
            m_users.insert({usr->user(), usr});
        }
    }
}

void AquaBox::readRules(const Path& p)
{
    m_rulesDir = p;
    m_rulesDir.append("rules");
    m_rules = RulesParser::parse(m_rulesDir);
}

std::shared_ptr<Device> AquaBox::findDevice(std::string id)
{
    auto p = m_devices.find(id);
    if (p == m_devices.end()) {
        return nullptr;
    }
    return p->second;
}

std::shared_ptr<Rule> AquaBox::findRule(std::string id)
{
    auto p = m_rules.find(id);
    if (p == m_rules.end()) {
        return nullptr;
    }
    return p->second;
}

const Path& AquaBox::workingDirectory() const
{
    return m_wd;
}

DevicesMap& AquaBox::devices() const
{
    return m_devices;
}

RulesMap& AquaBox::rules() const
{
    return m_rules;
}

bool AquaBox::addRule(std::shared_ptr<Rule>& rule, bool replace)
{
    auto pos = m_rules.find(rule->getId());
    nlohmann::json beforeUpdate;
    if (pos != m_rules.end()) {
        if (!replace) {
            LOG_ERROR << "Rule " << pos->first << " has already beed added, and replace flag was not set";
            return false;
        };
        pos->second->json(beforeUpdate);
        pos->second = rule;
    }
    else {
        m_rules.insert({ rule->getId(), rule });
    }

    Path p = m_rulesDir;

    auto device = rule->getDevice().resolve();

    bool pathOk = p.append((std::string)(rule->getId()) + ".json");
    if(!pathOk) {
        LOG_ERROR << "Can't form path for rule. Root: " << m_rulesDir.c_str() << " formed " << p.c_str() << " error " << strerror(errno);
        return false;
    }

    bool saved = rule->saveToFile(p);
    if (!saved) {
        m_rules.erase(m_rules.find(rule->getId()));
    }

    if (saved) {
        postUpdate(std::make_shared<DeviceRulesChangedEvent>(device));
        if (replace) {
            postUpdate(std::make_shared<RuleUpdatedEvent>(rule, beforeUpdate));
        }
        else {
            postUpdate(std::make_shared<RuleCreatedEvent>(rule));
        }
    }
    return saved;
}

bool AquaBox::removeRule(std::shared_ptr<Rule> rule)
{
    auto pos = m_rules.find(rule->getId());
    if (pos == m_rules.end()) {
        return false;
    }

    Path p = m_rulesDir;
    p.append((std::string)(rule->getId()) + ".json");
    if (!p.exists()) {
        LOG_ERROR << "Can't find rule " << rule->getId() << " tried: " << p.c_str();
        return false;
    }
    std::remove(p.c_str());

    if(p.exists()) {
        LOG_ERROR << "Can't remove file " << p.c_str();
        return false;
    }
    auto device = pos->second->getDevice().resolve();

    postUpdate(std::make_shared<RuleDeletedEvent>(pos->second));
    m_rules.erase(pos);
    postUpdate(std::make_shared<DeviceRulesChangedEvent>(device));
    return true;
}

bool AquaBox::updateDevice(std::string id)
{
    auto pos = m_devices.find(id);
    if (pos == m_devices.end()) {
        return false;
    }

    Path p = m_devicesDir;

    bool pathOk = p.append(id + ".json");
    if(!pathOk) {
        LOG_ERROR << "Can't form path for device. Root: " << m_devicesDir.c_str() << " formed " << p.c_str() << " error " << strerror(errno);
        return false;
    }

    return pos->second->saveToFile(p);
}

UsersMap& AquaBox::users() const
{
    return m_users;
}

void AquaBox::doLater(std::function<void(void)>&& task)
{
    if(!m_pool->tryPost(task)) {
        LOG_ERROR << "Can't schedule a new task";
    }
}

Cloud& AquaBox::cloud()
{
    return m_cloud;
}

void AquaBox::postUpdate(std::shared_ptr<Event> event)
{
    m_server.postUpdate(event);

    if (Options::EnableCloud) {
        m_cloud.postUpdate(event);
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
