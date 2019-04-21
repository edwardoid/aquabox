#ifndef RESTAPI_H
#define RESTAPI_H

#include <string>
#include <vector>
#include "log.h"
#include "route.h"
#include <server_http.hpp>

#define Access_Control_Allow_Origin "Access-Control-Allow-Origin"
#define Access_Control_Allow_Headers "Access-Control-Allow-Headers"
#define Access_Control_Allow_Methods "Access-Control-Allow-Methods"

#define DECLARE_API_IMPL(Name)   struct Name ## Impl { static void exec(const Route& route, Request& request, Response& response, Headers& headers); };
#define DEFINE_API_IMPL(Name) void Name ## Impl::exec(const Route& route, Request& request, Response& response, Headers& headers)
#define ENABLE_API(Name) inline void Name (const Route& route, Response rp, Request rq)                           \
                         {                                                                                        \
                           try {                                                                                  \
                             LOG_DEBUG << "API call " << rq->method << " " <<  rq->path << " handler " << #Name ;    \
                             Headers headers;                                                                     \
                             headers.emplace("Content-Type", "application/json");                                 \
                             headers.emplace(Access_Control_Allow_Origin, "*");                                 \
                             headers.emplace(Access_Control_Allow_Headers, "Origin, X-Requested-With, Content-Type, Accept"); \
                             headers.emplace(Access_Control_Allow_Methods, "POST, GET, PUT, PATCH, DELETE, OPTIONS"); \
                             Name ## Impl::exec(route, rq, rp, headers);                                          \
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
        using HttpServerImpl = SimpleWeb::Server<SimpleWeb::HTTP>;
        using Request = std::shared_ptr<HttpServerImpl::Request>; 
        using Response = std::shared_ptr<HttpServerImpl::Response>;
        using Headers = SimpleWeb::CaseInsensitiveMultimap;

        DECLARE_API_IMPL(OptionsOK);
        DECLARE_API_IMPL(Login);
        DECLARE_API_IMPL(StaticFiles);
        DECLARE_API_IMPL(Status);
        DECLARE_API_IMPL(StatusQR);
        DECLARE_API_IMPL(GetDevice);
        DECLARE_API_IMPL(Devices);
        DECLARE_API_IMPL(Rules);
        DECLARE_API_IMPL(GetRule);
        DECLARE_API_IMPL(CreateRule);
        DECLARE_API_IMPL(DeleteRule);
        DECLARE_API_IMPL(DeviceRules);
        DECLARE_API_IMPL(WiFiQR);
        DECLARE_API_IMPL(SupportedRepeatTimeouts);

        DECLARE_API_IMPL(TurnOn);
        DECLARE_API_IMPL(TurnOff);

        struct API
        {
          void init(HttpServerImpl& server);
          void setup(HttpServerImpl& server,
                     std::string method,
                     Route& route,
                     void (API::* Callback)(const aquabox::Route&, aquabox::rest::Response, aquabox::rest::Request));
          ENABLE_API(OptionsOK);
          ENABLE_API(Login)
          ENABLE_API(StaticFiles)
          ENABLE_API(Status)
          ENABLE_API(StatusQR)
          ENABLE_API(WiFiQR)
          ENABLE_API(GetDevice)
          ENABLE_API(Devices)
          ENABLE_API(Rules)
          ENABLE_API(GetRule)
          ENABLE_API(CreateRule)
          ENABLE_API(DeleteRule)
          ENABLE_API(DeviceRules)
          ENABLE_API(SupportedRepeatTimeouts)

          ENABLE_API(TurnOn);
          ENABLE_API(TurnOff);
        };
    }
}

#endif // RESTAPI_H