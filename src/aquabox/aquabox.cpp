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
#include "status_image.h"
#include "options.h"
#include <exception>
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
    , m_logAppender("./static/logs/aquabox.csv", LOG_FILE_SIZE_IN_MB * BYTES_IN_MB, 1)
    , m_globalTimer("GlobalRoutines")
{}

int AquaBox::start()
{
    Lockfile lock;

    plog::init( plog::severityFromString(Settings::string(Settings::Keys::LOG_LEVEL, plog::severityToString(plog::Severity::info)).c_str()),
                &m_consoleAppender);

    plog::init( plog::severityFromString(Settings::string(Settings::Keys::LOG_LEVEL, plog::severityToString(plog::Severity::info)).c_str()),
                &m_logAppender);
    
    if (Options::EnableCloud) {
        plog::init( plog::severityFromString(Settings::string(Settings::Keys::LOG_LEVEL, plog::severityToString(plog::Severity::info)).c_str()),
                &m_cloudAppender);
    }

    if (Options::Service) {
        plog::init( plog::severityFromString(Settings::string(Settings::Keys::LOG_LEVEL, plog::severityToString(plog::Severity::info)).c_str()),
                &m_fileAppender);
    }

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

    m_poolOptions.setQueueSize(Settings::integer(Settings::Keys::THREADS, std::thread::hardware_concurrency()));
    m_poolOptions.setQueueSize(1024);

    m_pool.reset(new tp::ThreadPool(m_poolOptions));
    
    m_wifi.reset(new IoT::WiFi());
    m_wifi->onStateChanged([this](IoT::WiFi::State state) {
        networkStateChanged(state);
    });

    m_wifi->init("",
                 Settings::string(Settings::Keys::AP_SSID, Settings::Defaults::AP_SSID),
                 Settings::string(Settings::Keys::AP_PASSWORD, Settings::Defaults::AP_PASSWORD),
                 Options::AccessPoint);

    readDevices(m_wd);
    readRules(m_wd);
    readUsers();

    StatusImage::display();
    m_globalTimer.start([this](const Timer::ClockTimePoint&){
        if (Options::Render) {
            StatusImage::display();
        }
        if (Options::EnableCloud) {
            m_cloud.heartheat();
        }
        SensorsTracker::processUpdates();

        reconnectIfNeeded();
    }, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::seconds(10)));

    m_timer.start();
    m_server.start();

    std::getchar();

    return EXIT_SUCCESS;
}

void AquaBox::networkStateChanged(IoT::WiFi::State state)
{
    if (state == IoT::WiFi::State::InAPMode) {
        static bool firstTime = true;
        static std::time_t last = 0;
        std::time_t now = std::time(nullptr);
        bool reconnect = (now - last) > 60 * 60; 
        std::string uuid = Settings::string(Settings::Keys::NETWORK, "");
        if(reconnect && !uuid.empty()) {
            m_wifi->connectToNetwork(uuid);
        }
    }
}

void AquaBox::reconnectIfNeeded()
{
    static std::time_t last = 0;
    std::time_t now = std::time(nullptr);
    bool reconnect = (now - last) > 60; 
    if (m_wifi->state() != IoT::WiFi::State::Disconnected && reconnect) {
        return;
    }
    std::string uuid = Settings::string(Settings::Keys::NETWORK, "");
    if(!uuid.empty()) {
        LOG_INFO << "Network is down, trying to connect to " << uuid;
        m_wifi->connectToNetwork(uuid);
    }
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

std::vector<IoT::WifiNetwork> AquaBox::availableNetworks()
{
    return std::move(m_wifi->availableNetworks());
}

IoT::WiFi& AquaBox::networking()
{
    return *m_wifi;
}

Cloud& AquaBox::cloud()
{
    return m_cloud;
}

void AquaBox::postUpdate(std::shared_ptr<Event> event)
{
    m_server.postUpdate(event);
}
