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

#pragma once

#include "bus.h"
#include <thread>
#include <memory>
#include <map>

namespace aquabox
{
    namespace proto
    {
        class MessageIO;
    }

    class HermesBus: public Bus
    {
    public:
        HermesBus();
        std::shared_ptr<Hardware> hw(const std::string& hw) override;
    protected:
        virtual void loop() = 0;
        virtual void newConnection(std::shared_ptr<proto::MessageIO> io);
    private:
        std::thread m_thread;
        std::map<int, std::shared_ptr<class HermesHardware>> m_master;
    };
}