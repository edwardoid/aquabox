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
                             Name ## Impl().exec(rq, rp);                                           \
                           } catch (const std::exception& e) {                                      \
                             LOG_ERROR << "An error occured during API call. Details: " << e.what();\
                           } catch (...) {                                                          \
                             LOG_ERROR << "Unknown error occured during API call";                  \
                           }                                                                        \
                         }

namespace aquabox
{
    namespace rest
    {
        using Request = Pistache::Rest::Request; 
        using Response = Pistache::Http::ResponseWriter;

        DECLARE_API_IMPL(Status);
        DECLARE_API_IMPL(WiFiQR);

        struct API
        {
            ENABLE_API(Status)
            ENABLE_API(WiFiQR)
        };
    }
}

#endif // RESTAPI_H