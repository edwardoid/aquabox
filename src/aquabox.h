#ifndef AQUABOX_H
#define AQUABOX_H

#include "identifiable.h"
#include "pistache/description.h"
#include "pistache/router.h"
#include "pistache/endpoint.h"
#include "restapi.h"
#include <memory>

namespace aquabox
{
    class AquaBox: public Identifiable
    {
        AquaBox(const AquaBox&) = delete;
        AquaBox(AquaBox&&) = delete;
        AquaBox& operator = (const AquaBox&) = delete;
    public:
        AquaBox();
        int start();
        ~AquaBox() = default;
    private:
        Pistache::Rest::Description initRestApi();
    private:
        std::unique_ptr<Pistache::Http::Endpoint> m_server;
        Pistache::Rest::Description m_desc;
        Pistache::Rest::Router m_router;
        rest::API REST;
    };
}

#endif // AQUABOX_H