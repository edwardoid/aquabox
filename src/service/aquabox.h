#ifndef AQUABOX_H
#define AQUABOX_H

#include "identifiable.h"
#include "pistache/description.h"
#include "pistache/router.h"
#include "pistache/endpoint.h"
#include "restapi.h"
#include "device.h"
#include "user.h"
#include "web.h"
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
        const std::vector<Device>& devices() const;
        const std::vector<User>& users() const;
    private:
        Pistache::Rest::Description initRestApi();
        void readDevices();
        void readUsers();
    private:
        plog::ColorConsoleAppender<plog::TxtFormatter> m_consoleAppender;
        std::shared_ptr<Pistache::Http::Endpoint> m_server;
        std::vector<Device> m_devices;
        std::vector<User> m_users;
        Pistache::Rest::Description m_desc;
        Pistache::Rest::Router m_router;
        ServeDir m_webServer;
        rest::API REST;
    };
}

#endif // AQUABOX_H