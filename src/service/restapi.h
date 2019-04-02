#ifndef RESTAPI_H
#define RESTAPI_H

#include <string>
#include <vector>
#include "log.h"
#include "pistache/http.h"
#include "pistache/router.h"
#include "pistache/description.h"


#define DECLARE_API_IMPL(Name)   struct Name ## Impl { void exec(const Request& request, Response& response); };
#define DEFINE_API_IMPL(Name) void Name ## Impl::exec(const Request& request, Response& response)
#define ENABLE_API(Name) inline void Name (const Request& rq, Response rp)                          \
                         {                                                                          \
                           try {                                                                    \
                             LOG_DEBUG << "API call " << rq.method() << " " <<  rq.resource();      \
                             rp .headers().add(std::make_shared<Pistache::Http::Header::AccessControlAllowOrigin>("*"));   \
                             Name ## Impl().exec(rq, rp);                                                         \
                           } catch (std::exception e) {                                                           \
                             LOG_ERROR << "An error occured during API call. Details: " << e.what();              \
                           } catch (...) {                                                                        \
                             LOG_ERROR << "Unknown error occured during API call. Details: "  << strerror(errno); \
                           }                                                                                      \
                         }                                                                                        \

namespace aquabox
{
    namespace rest
    {
        using Request = Pistache::Rest::Request; 
        using Response = Pistache::Http::ResponseWriter;

        DECLARE_API_IMPL(WhoAmI);
        DECLARE_API_IMPL(Login);
        DECLARE_API_IMPL(Status);
        DECLARE_API_IMPL(Devices);
        DECLARE_API_IMPL(Rules);
        DECLARE_API_IMPL(GetRule);
        DECLARE_API_IMPL(CreateRule);
        DECLARE_API_IMPL(DeviceRules);
        DECLARE_API_IMPL(WiFiQR);
        DECLARE_API_IMPL(SupportedRepeatTimeouts);

        struct API
        {
            ENABLE_API(WhoAmI);
            ENABLE_API(Login)
            ENABLE_API(Status)
            ENABLE_API(WiFiQR)
            ENABLE_API(Devices)
            ENABLE_API(Rules)
            ENABLE_API(GetRule)
            ENABLE_API(CreateRule)
            ENABLE_API(DeviceRules)
            ENABLE_API(SupportedRepeatTimeouts)
        };
    }
}

#endif // RESTAPI_H