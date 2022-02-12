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

#include <iot_definitions.h>
#include <route.h>

namespace aquabox
{
    class HardwareApi
    {
        HardwareApi(){};
        HardwareApi(const HardwareApi&) = delete;
        HardwareApi(HardwareApi&&) = delete;
        HardwareApi& operator=(const HardwareApi&) = delete;
        HardwareApi& operator=(HardwareApi&&) = delete;

    public:
        static bool start();

    private:
        bool acceptConnectionFromHW(std::string id, std::shared_ptr<Web::WSConnection> connection);
        void setupRoutes();

    private:
        static HardwareApi self;
        Web::HttpServer m_http;
        Web::WSServer m_ws;
        Route m_register;
        Route m_login;
        Route m_firmware;
        Route m_events;
        Route m_commands;
    };
} // namespace aquabox
