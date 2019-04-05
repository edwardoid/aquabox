#ifndef AQUABOX_H
#define AQUABOX_H

#include "identifiable.h"
#include <thread_pool.hpp>
#include "restapi.h"
#include "device.h"
#include "rule.h"
#include "user.h"
#include "path.h"
#include "rulestimer.h"
#include "webserver.h"
#include <memory>

namespace aquabox
{
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
        //Pistache::Rest::Description initRestApi();
        void readDevices();
        void readUsers();
        void readRules(const Path& root);
    private:
        plog::ColorConsoleAppender<plog::TxtFormatter> m_consoleAppender;
        //std::shared_ptr<Pistache::Http::Endpoint> m_server;
        tp::ThreadPoolOptions m_poolOptions;
        std::unique_ptr<tp::ThreadPool> m_pool;
        DevicesMap m_devices;
        UsersMap m_users;
        Path m_rulesDir;
        RulesMap m_rules;
        RulesTimer m_timer;
        WebServer m_server;
        //Pistache::Rest::Description m_desc;
        //Pistache::Rest::Router m_router;
        //rest::API REST;
    };
}

#endif // AQUABOX_H