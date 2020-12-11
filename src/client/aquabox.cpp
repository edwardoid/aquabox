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
#include "settings.h"
#include "lock_file.h"
#include "log.h"
#include "rest_api.h"
#include "rule.h"
#include "device.h"
#include "rule_parser.h"
#include "device_parser.h"
#include "user.h"
#include "update_events.h"
#include "network_events.h"
#include "options.h"
#include "repetive_task.h"
#include "cloud.h"
#include "web_server.h"
#include "network_manager.h"
#include "screen_manager.h"
#include "global_timer.h"
#include "rules_timer.h"
#include <singleton.h>
#include <pstream.h>
#include <exception>
#include <chrono>
#include <random>

#define LOG_FILE_SIZE_IN_MB 10
#define BYTES_IN_MB 1000000
#define MAX_LOG_FILES 50

using namespace aquabox;

AquaBox::AquaBox()
    : m_pool(Settings::integer(Settings::Keys::THREADS, std::thread::hardware_concurrency())), m_fileAppender("./../log/aquabox.log", LOG_FILE_SIZE_IN_MB * BYTES_IN_MB, MAX_LOG_FILES), m_rulesTimer(new RulesTimer()), m_server(new WebServer)
{
    //PerformanceMetrics::update();
}

AquaBox::~AquaBox()
{
    delete m_rulesTimer;
    delete m_server;
}

int AquaBox::start()
{
    Lockfile lock;

    plog::init(plog::severityFromString(Settings::string(Settings::Keys::LOG_LEVEL, plog::severityToString(plog::Severity::info)).c_str()),
               &m_consoleAppender);

    if (!Options::Service)
    {
        plog::init(plog::severityFromString(Settings::string(Settings::Keys::LOG_LEVEL, plog::severityToString(plog::Severity::info)).c_str()),
                   &m_fileAppender);
    }

    if (Singleton<NetworkManager>::available)
    {
        auto nm = Singleton<NetworkManager>::i();
        nm->setInterface(Settings::string(Settings::Keys::CLIENT_INTERFACE, Settings::Defaults::CLIENT_INTERFACE));
        nm->currentConnection.connect([this](Connection c) { postUpdate(std::make_shared<NetworkChangedEvent>(c)); });
        nm->deviceState.connect([this](NetworkDeviceState state) { postUpdate(std::make_shared<NetworkDeviceStateEvent>(state)); });
    }

    LOG_INFO << "Starting " << Settings::Defaults::APPLICATION << " ver. " << std::hex << Settings::Defaults::VERSION_MAJOR << "." << Settings::Defaults::VERSION_MINOR;
    if (Options::Lock)
    {
        if (!lock.create())
        {
            LOG_FATAL << "Another instance of " << Settings::Defaults::APPLICATION << " is running...";
            std::exit(EXIT_FAILURE);
        }
    }

    srand(time(NULL));
    std::string procPath = "/proc/";
    procPath += std::to_string(static_cast<int>(getpid()));
    procPath += "/exe";

    char cwd[1024];
    memset(cwd, '\0', 1024);

    int bytes = std::min<size_t>(readlink(procPath.c_str(), cwd, 1024), 1024 - 1);
    if (bytes >= 0)
    {
        cwd[bytes] = '\0';
    }

    m_wd = Settings::string(Settings::Keys::WORKING_DIRECTORY, cwd);
    if (!m_wd.normalize())
    {
        LOG_ERROR << "Bad working directory path " << m_wd;
    }
    else
    {
        if (0 != chdir(m_wd.c_str()))
        {
            LOG_ERROR << "Can't change working directory to " << m_wd << " errno: " << errno << " : " << strerror(errno);
        }
        else
        {
            LOG_INFO << "Changed working direcotry to " << m_wd;
        }
    }

    readDevices(m_wd);
    readRules(m_wd);
    readUsers();

    Singleton<ScreenManager>::i(); // Create manager if needed!
    m_globalTimer = std::make_unique<GlobalTimer>();

    m_rulesTimer->start();
    m_server->start();

    m_globalTimer->waitToFinish();

    return EXIT_SUCCESS;
}

void AquaBox::readDevices(const Path &p)
{
    m_devicesDir = p;
    m_devicesDir.append("devices");
    m_devices.set(DeviceParser::parse(m_devicesDir));
}

void AquaBox::readUsers()
{
    auto users = Settings::array(Settings::Keys::USERS);

    LOG_INFO << "Loading " << users.size() << " users...";
    for (auto u : users)
    {
        auto usr = std::make_shared<User>();
        if (usr->init(u))
        {
            m_users.insert(usr->user(), usr);
        }
    }
}

void AquaBox::readRules(const Path &p)
{
    m_rulesDir = p;
    m_rulesDir.append("rules");
    m_rules.set(RulesParser::parse(m_rulesDir));
}

std::shared_ptr<Device> AquaBox::findDevice(const std::string &id)
{
    return m_devices.get(id, nullptr);
}

std::shared_ptr<Rule> AquaBox::findRule(const std::string &id)
{
    return m_rules.get(id, nullptr);
}

const Path &AquaBox::workingDirectory() const
{
    return m_wd;
}

DevicesMap &AquaBox::devices() const
{
    return m_devices;
}

RulesMap &AquaBox::rules() const
{
    return m_rules;
}

bool AquaBox::addRule(std::shared_ptr<Rule> &rule, bool replace)
{
    nlohmann::json beforeUpdate;

    bool replaceError = false;

    if (m_rules.on(rule->getId(), [this, rule, &beforeUpdate, replace, &replaceError](auto &k, auto &v) {
            replaceError = !replace;

            if (!replaceError)
            {
                v->json(beforeUpdate);
                v = rule;
            }
        }))
    {
        if (replaceError)
        {
            LOG_ERROR << "Rule " << rule->getId() << " has already beed added, and replace flag was not set";
            return false;
        };
    }
    else
    {
        m_rules.insert(rule->getId(), rule);
    }

    Path p = m_rulesDir;

    auto device = rule->getDevice().resolve();

    bool pathOk = p.append((std::string)(rule->getId()) + ".json");
    if (!pathOk)
    {
        LOG_ERROR << "Can't form path for rule. Root: " << m_rulesDir.c_str() << " formed " << p.c_str() << " error " << strerror(errno);
        return false;
    }

    bool saved = rule->saveToFile(p);
    if (!saved)
    {
        m_rules.erase(rule->getId());
    }

    if (saved)
    {
        postUpdate(std::make_shared<DeviceRulesChangedEvent>(device));
        if (replace)
        {
            postUpdate(std::make_shared<RuleUpdatedEvent>(rule, beforeUpdate));
        }
        else
        {
            postUpdate(std::make_shared<RuleCreatedEvent>(rule));
        }
    }
    return saved;
}

bool AquaBox::removeRule(const std::string &id)
{
    if (!m_rules.erase(id))
    {
        return false;
    }

    std::shared_ptr<Device> device;
    m_rules.on<std::shared_ptr<Device>>(
        id, [this](auto id, auto rule) {
            postUpdate(std::make_shared<RuleDeletedEvent>(rule));
            return rule->getDevice().resolve();
        },
        device);

    if (m_rules.erase(id))
    {
        postUpdate(std::make_shared<DeviceRulesChangedEvent>(device));
    }

    Path p = m_rulesDir;
    p.append(id + ".json");
    if (!p.exists())
    {
        LOG_ERROR << "Can't find rule " << id << " tried: " << p.c_str();
        return false;
    }
    std::remove(p.c_str());

    if (p.exists())
    {
        LOG_ERROR << "Can't remove file " << p.c_str();
        return false;
    }
    return true;
}

bool AquaBox::updateDevice(std::string id)
{
    Path p = m_devicesDir;

    bool pathOk = p.append(id + ".json");
    if (!pathOk)
    {
        LOG_ERROR << "Can't form path for device. Root: " << m_devicesDir.c_str() << " formed " << p.c_str() << " error " << strerror(errno);
        return false;
    }

    return m_devices.on(id, [p] (auto id, auto dev) {
        dev->saveToFile(p);
    });
}

UsersMap &AquaBox::users() const
{
    return m_users;
}

void AquaBox::doLater(const std::string &name, std::function<void(void)> &&task)
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

    if (c != nullptr)
    {
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
    configurationDataString += std::to_string(Settings::integer(Settings::Keys::HTTP_PORT, Settings::Defaults::REST_PORT));
    configurationDataString += ";";

    configurationDataString += std::to_string(Settings::integer(Settings::Keys::STREAM_PORT, Settings::Defaults::WS_PORT));
    configurationDataString += ";";

    configurationDataString += SERIAL_NUMBER;
    configurationDataString += ";";

    if (Singleton<NetworkManager>::i() != nullptr)
        configurationDataString += Singleton<NetworkManager>::i()->currentConnection.value.ip;
    else
        configurationDataString += "aquabox";
    configurationDataString += ";";

    configurationDataString += std::to_string((int)Singleton<AquaBox>::get()->systemTemperature());
    return configurationDataString;
}
