#ifndef AQUABOX_H
#define AQUABOX_H

#include "identifiable.h"
#include <thread_pool.hpp>
#include "rulestimer.h"
#include "webserver.h"
#include "path.h"
#include <memory>
#include <unordered_map>

namespace aquabox
{
    class Device;
    class Rule;
    class User;
    class Event;

    using DevicesMap = std::unordered_map<std::string, std::shared_ptr<Device>>;
    using RulesMap = std::unordered_map<std::string, std::shared_ptr<Rule>>;
    using UsersMap = std::unordered_map<std::string, std::shared_ptr<User>>;

    class AquaBox: public Identifiable
    {
        AquaBox(const AquaBox&) = delete;
        AquaBox(AquaBox&&) = delete;
        AquaBox& operator = (const AquaBox&) = delete;
        AquaBox();
        static AquaBox Instance;
    public:
        static AquaBox& i();
        int start(bool isService = false);
        ~AquaBox() = default;
        std::shared_ptr<Device> findDevice(std::string id);
        std::shared_ptr<Rule> findRule(std::string id);
        const Path& workingDirectory() const;
        DevicesMap& devices() const;
        UsersMap& users() const;
        RulesMap& rules() const;
        bool addRule(std::shared_ptr<Rule>& rule, bool replace = false);
        bool removeRule(std::shared_ptr<Rule> rule);
        void doLater(std::function<void(void)>&& task);
        void postUpdate(std::shared_ptr<Event> event);
    private:
        void readDevices();
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
        Path m_wd;
        Path m_rulesDir;
        RulesTimer m_timer;
        WebServer m_server;
        Timer m_globalTimer;
    };
}

#endif // AQUABOX_H