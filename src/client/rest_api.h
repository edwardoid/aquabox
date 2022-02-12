/*
 * AquaBox
 * Copyright (C) 2020  Eduard Sargsyan
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

#pragma once

#include <common/api.h>

namespace aquabox
{
    using namespace Web;
    namespace v1
    {
        class REST : public API
        {
        public:
            virtual void Status(const Route& route, Request& request, Response& response, Headers& headers) override;
            virtual void ScanWiFiNetworks(const Route& route, Request& request, Response& response, Headers& headers) override;
            virtual void ListNetworks(const Route& route, Request& request, Response& response, Headers& headers) override;
            virtual void ConnectToWiFi(const Route& route, Request& request, Response& response, Headers& headers) override;
            virtual void GetDevice(const Route& route, Request& request, Response& response, Headers& headers) override;
            virtual void UpdateDevice(const Route& route, Request& request, Response& response, Headers& headers) override;
            virtual void Devices(const Route& route, Request& request, Response& response, Headers& headers) override;
            virtual void Rules(const Route& route, Request& request, Response& response, Headers& headers) override;
            virtual void GetRule(const Route& route, Request& request, Response& response, Headers& headers) override;
            virtual void CreateRule(const Route& route, Request& request, Response& response, Headers& headers) override;
            virtual void DeleteRule(const Route& route, Request& request, Response& response, Headers& headers) override;
            virtual void DeviceRules(const Route& route, Request& request, Response& response, Headers& headers) override;
            virtual void SupportedRepeatTimeouts(const Route& route, Request& request, Response& response, Headers& headers) override;
            virtual void SetValue(const Route& route, Request& request, Response& response, Headers& headers) override;
            virtual void Call(const Route& route, Request& request, Response& response, Headers& headers) override;
            virtual void GetHistory(const Route& route, Request& request, Response& response, Headers& headers) override;

            virtual void init(std::shared_ptr<HttpServer> http, Route base) override;
            void StatusQR(const Route& route, Request& request, Response& response, Headers& headers);

        private:
            Route m_statusQR;
        };
    } // namespace v1
} // namespace aquabox
