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
        int start();
        ~AquaBox() = default;
        std::shared_ptr<Device> findDevice(std::string id);
        std::shared_ptr<Rule> findRule(std::string id);
        DevicesMap devices() const;
        UsersMap users() const;
        RulesMap rules() const;
        bool addRule(std::shared_ptr<Rule>& rule, bool replace = false);
        void doLater(std::function<void(void)>&& task);
    private:
        void readDevices();
        void readUsers();
        void readRules(const Path& root);
    private:
        plog::ColorConsoleAppender<plog::TxtFormatter> m_consoleAppender;
        tp::ThreadPoolOptions m_poolOptions;
        std::unique_ptr<tp::ThreadPool> m_pool;
        DevicesMap m_devices;
        UsersMap m_users;
        Path m_rulesDir;
        RulesMap m_rules;
        RulesTimer m_timer;
        WebServer m_server;
    };
}

#endif // AQUABOX_H