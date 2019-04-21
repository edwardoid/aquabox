#include "aquabox.h"
#include "config.h"
#include "settings.h"
#include "lockfile.h"
#include "log.h"
#include "restapi.h"
#include "rule.h"
#include "ruleparser.h"
#include "deviceparser.h"
#include "user.h"
#include "updatevents.h"
#include <exception>
#include <random>

using namespace aquabox;

AquaBox AquaBox::Instance;


AquaBox& AquaBox::i()
{
    return Instance;
}

AquaBox::AquaBox()
    : m_fileAppender("aquabox.log")
{}

int AquaBox::start(bool asService)
{
    Lockfile lock;

    plog::init( plog::severityFromString(Settings::string(Settings::LOG_LEVEL, plog::severityToString(plog::Severity::info)).c_str()),
                &m_consoleAppender);
    
    if (asService) {
        plog::init( plog::severityFromString(Settings::string(Settings::LOG_LEVEL, plog::severityToString(plog::Severity::info)).c_str()),
                &m_fileAppender);
    }

    LOG_INFO << "Starting " << Config::APPLICATION << " ver. " << std::hex << Config::VERSION_MAJOR << "." << Config::VERSION_MINOR;
    if(lock.exists())
    {
        LOG_FATAL << "Another instance of " << Config::APPLICATION << " is running...";
        std::exit(EXIT_FAILURE);
    }

    srand(time(NULL));
    std::string procPath = "/proc/";
    procPath += std::to_string(static_cast<int>(getpid()));
    procPath += "/exe";

    char cwd[1024];
    memset(cwd,'\0', 1024);

    int bytes = std::min<size_t>(readlink(procPath.c_str(), cwd, 1024), 1024 - 1);
    if(bytes >= 0)
        cwd[bytes] = '\0';

    m_wd = Settings::string(Settings::WORKING_DIRECTORY, cwd);
    if (!m_wd.normalize())
    {
        LOG_ERROR << "Bad working directory path " << m_wd;
    }
    else
    {
        if(0 != chdir(m_wd.c_str()))
        {
            LOG_ERROR << "Can't change working directory to " << m_wd << " errno: " << errno << " : " << strerror(errno);
        }
        else
        {
            LOG_INFO << "Changed workign direcotry to " << m_wd;
        }
    }

    m_poolOptions.setQueueSize(Settings::integer(Settings::THREADS, std::thread::hardware_concurrency()));
    m_poolOptions.setQueueSize(1024);

    m_pool.reset(new tp::ThreadPool(m_poolOptions));

    readDevices();
    readUsers();
    readRules(m_wd);

    m_timer.start();
    m_server.start();
    
    m_server.start();

    std::getchar();

    return EXIT_SUCCESS;
}
void AquaBox::readDevices()
{
    m_devices = DeviceParser::parse();
}

void AquaBox::readUsers()
{
    auto users = Settings::array(Settings::USERS);
    
    LOG_INFO << "Loading " << users.size() << " users...";
    for (auto u : users)
    {
        auto usr = std::make_shared<User>();
        if(usr->init(u))
            m_users.insert({usr->user(), usr});
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
    if (p == m_devices.end())
        return nullptr;
    return p->second;
}

std::shared_ptr<Rule> AquaBox::findRule(std::string id)
{
    auto p = m_rules.find(id);
    if (p == m_rules.end())
        return nullptr;
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
    auto pos = m_rules.find(rule->getId().Value);
    if (pos != m_rules.end()) {
        if (!replace)
        {
            LOG_ERROR << "Rule " << pos->first << " has already beed added, and replace flag was not set";
            return false;
        };
        pos->second = rule;
    }
    else {
        m_rules.insert({ rule->getId().Value, rule });
    }

    Path p = m_rulesDir;

    auto device = rule->getDevice().resolve();

    bool pathOk = p.append(rule->getId().Value + ".json");
    if(!pathOk) {
        LOG_ERROR << "Can't form path for rule. Root: " << m_rulesDir.c_str() << " formed " << p.c_str() << " error " << strerror(errno);
        return false;
    }

    bool saved =  rule->saveToFile(p);
    if (!saved) {
        m_rules.erase(m_rules.find(rule->getId().Value));
    }

    if (saved) {
        postUpdate(std::make_shared<DeviceRulesChangedEvent>(device));
        if (replace)
            postUpdate(std::make_shared<RuleUpdatedEvent>(rule));
        else 
            postUpdate(std::make_shared<RuleCreatedEvent>(rule));
    }
    return saved;
}

bool AquaBox::removeRule(std::shared_ptr<Rule> rule)
{
    auto pos = m_rules.find(rule->getId().Value);
    if (pos == m_rules.end()) {
        return false;
    }

    Path p = m_rulesDir;
    p.append(rule->getId().Value + ".json");
    if (!p.exists()) {
        LOG_ERROR << "Can't find rule " << rule->getId().Value << " tried: " << p.c_str();
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

UsersMap& AquaBox::users() const
{
    return m_users;
}

void AquaBox::doLater(std::function<void(void)>&& task)
{
    if(!m_pool->tryPost(task))
    {
        LOG_ERROR << "Can't schedule a new task";
    }
}

void AquaBox::postUpdate(std::shared_ptr<Event> event)
{
    m_server.postUpdate(event);
}