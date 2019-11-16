#ifndef API_H
#define API_H

#include "route.h"
#include "log.h"
#include <memory>
#include <server_http.hpp>

namespace aquabox
{
    namespace v1
    {
        class API
        {
        public:
            using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
            using Request = std::shared_ptr<HttpServer::Request>; 
            using Response = std::shared_ptr<HttpServer::Response>;
            using Headers = SimpleWeb::CaseInsensitiveMultimap;

            virtual void init(std::shared_ptr<HttpServer> http, Route base);
            void enableCORS(bool enable);

            virtual void defaultHandler(const Route& route, Request& request, Response& response, Headers& headers);
            
            virtual void Status(const Route& route, Request& request, Response& response, Headers& headers) = 0;
            virtual void ScanWiFiNetworks(const Route& route, Request& request, Response& response, Headers& headers) = 0;
            virtual void ConnectToWiFi(const Route& route, Request& request, Response& response, Headers& headers) = 0;
            virtual void GetDevice(const Route& route, Request& request, Response& response, Headers& headers) = 0;
            virtual void UpdateDevice(const Route& route, Request& request, Response& response, Headers& headers) = 0;
            virtual void Devices(const Route& route, Request& request, Response& response, Headers& headers) = 0;
            virtual void Rules(const Route& route, Request& request, Response& response, Headers& headers) = 0;
            virtual void GetRule(const Route& route, Request& request, Response& response, Headers& headers) = 0;
            virtual void CreateRule(const Route& route, Request& request, Response& response, Headers& headers) = 0;
            virtual void DeleteRule(const Route& route, Request& request, Response& response, Headers& headers) = 0;
            virtual void DeviceRules(const Route& route, Request& request, Response& response, Headers& headers) = 0;
            virtual void SupportedRepeatTimeouts(const Route& route, Request& request, Response& response, Headers& headers) = 0;
            virtual void TurnOn(const Route& route, Request& request, Response& response, Headers& headers) = 0;
            virtual void TurnOff(const Route& route, Request& request, Response& response, Headers& headers) = 0;


            using APiInterfaceCallback = void (API::*)(const Route&, Request&, Response&, Headers&);

            template<class ApiImplementation>
            struct Implementation
            {
                using APIImplementationCallback = void (ApiImplementation::*)(const Route&, Request&, Response&, Headers&);
            };

            template<typename ApiImplementation>
            APiInterfaceCallback apiCallback(typename Implementation< ApiImplementation>::APIImplementationCallback callback)
            {
                return reinterpret_cast<APiInterfaceCallback>(callback);
            }
        protected:
            virtual void CORSOptions(Request request, Response response);
        protected: 
            std::shared_ptr<HttpServer> server() const;
            void setup(std::string method, const Route&, void (API::* Callback)(const Route&, Request&, Response&, Headers&));

        private:
            void addCORSHeaders(Headers& headers);
            void dispatcher(const Route&, void (API::* Callback)(const Route&, Request&, Response&, Headers&),
                            Request request, Response response);
        private:
            std::shared_ptr<HttpServer> m_server;
            bool m_allowCORS = true;
            Route m_status;
            Route m_wifiScan;
            Route m_wifiConnect;

            Route m_updates;
            Route m_rpc;
            Route m_static;
            Route m_login;
            Route m_statusQR;
            Route m_allDevices;
            Route m_deviceById;
            Route m_updateDeviceById;
            Route m_allRules;
            Route m_ruleById;
            Route m_createRule;
            Route m_deleteRule;
            Route m_updateRule;
            Route m_rulesForDevice;
            Route m_repeatUnits;

            Route m_turnOn;
            Route m_turnOff;
        };
    }
}

#endif // API_H