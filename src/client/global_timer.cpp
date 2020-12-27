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

#include "global_timer.h"
#include "log.h"
#include "cloud.h"
#include "sensors_tracker.h"
#include "options.h"
//#include "performance_metrics.h"
#include <settings.h>
#include <singleton.h>

using namespace aquabox;

GlobalTimer::GlobalTimer()
    : Timer("GlobalRoutines")
{
    start([this](const Timer::ClockTimePoint&) {
        auto cloud = Singleton<Cloud>::i();
        if (cloud) {
            cloud->heartheat();
        }
        SensorsTracker::processUpdates();
    }, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::seconds(1)));
}

void GlobalTimer::waitToFinish()
{
    wait();
}
