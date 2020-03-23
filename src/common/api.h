/*
 * AquaBox
 * Copyright (C) 2019  Eduard Sargsyan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef API_H
#define API_H

#include "route.h"
#include "log.h"
#include "iot_definitions.h"

namespace aquabox
{
    using namespace Web;
    namespace v1
    {
        class API
        {
        public:

            virtual void init(std::shared_ptr<HttpServer> http, Route base);
            void enableCORS(bool enable);

            virtual void defaultHandler(const Route& route, Request& request, Response& response, Headers& headers);
            
            virtual void Status(const Route& route, Request& request, Response& response, Headers& headers) = 0;
            virtual void ScanWiFiNetworks(const Route& route, Request& request, Response& response, Headers& headers) = 0;
            virtual void ListNetworks(const Route& route, Request& request, Response& response, Headers& headers) = 0;
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
            Route m_wifiNetworks;
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