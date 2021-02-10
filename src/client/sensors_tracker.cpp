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

#include "sensors_tracker.h"
#include "sensor.h"
#include "aquabox.h"
#include "devices_map.h"
#include "events/update_events.h"
#include <unordered_map>

using namespace aquabox;

void SensorsTracker::processUpdates() {
    static std::unordered_map<std::string, double> values;

    Singleton<AquaBox>::get()->doLater("seonsors_tracker", []() {
        auto devices = Singleton<DevicesMap>::i()->ids();
        for (auto id : devices) {
            auto dev = Singleton<DevicesMap>::i()->get(id);
            if (dev == nullptr) {
                continue;
            }

            auto sensor = std::dynamic_pointer_cast<AbstractSensor>(dev);
            if (sensor == nullptr) {
                continue;
            }
            auto newValue = sensor->value();
            if (values.find(sensor->id()) != values.end()) {
                if(values[sensor->id()] == newValue) {
                    continue;
                }

                values[sensor->id()] = newValue;
            }
            else {
                values.insert( {sensor->id(), newValue} );
            }

            Singleton<AquaBox>::get()->postUpdate(std::make_shared<SensorChangedEvent>(sensor, "regular_check"));
        }
    });
}