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

#include "hermes_master.h"
#include <vector>
#include <mutex>
#include <memory>
#include <s2s.h>
#include <thread>
#include <chrono>
namespace aquabox
{
    class HermesIncubator
    {
    public:
        HermesIncubator();
        int incubate(std::shared_ptr<HermesMaster> obj);
        SignalSlot::Signal<int, std::shared_ptr<HermesMaster>> ready;
        SignalSlot::Signal<int, std::shared_ptr<HermesMaster>> dead;
    protected:
        void routine();
    private:
        enum ObjectInfoState
        {
            Incubating = 0,
            Ready = 1,
            Dead = 2
        };
        struct HermesObjectInfo
        {
            int id;
            std::shared_ptr<HermesMaster> object;
            ObjectInfoState state;
            std::chrono::time_point<std::chrono::high_resolution_clock> tp;
        };

        std::vector<HermesObjectInfo> m_objects;
        std::thread m_routine;
        std::mutex m_mx;
    };
}