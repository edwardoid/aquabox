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
#include <types/command.h>
#include <types/event.h>
#include <types/error_response.h>
#include <unordered_map>
#include "box_connection.h"
#include "api_calls_handler.h"

namespace aquabox
{
    class HardwareApi
    {
        HardwareApi() {};
        HardwareApi(const HardwareApi&) = delete;
        HardwareApi(HardwareApi&&) = delete;
        HardwareApi& operator=(const HardwareApi&) = delete;
        HardwareApi& operator=(HardwareApi&&) = delete;
    public:
        static bool start(ApiCallsHandler* handler);
        static std::shared_ptr<BoxConnection> box(const std::string& boxId);
    private:
        bool acceptConnectionFromHW(const std::string& id, std::shared_ptr<Web::WSConnection> connection);
        void setupRoutes();
        
    private:
        static HardwareApi self;
        std::unordered_map<std::string /* box id */, std::shared_ptr<BoxConnection>> m_boxes;
        ApiCallsHandler* m_handler;
        Web::WSServer m_ws;
        Route m_url;
    };
} // namespace aquabox
