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
    static std::unordered_map<std::string, nlohmann::json> values;

    Singleton<AquaBox>::get()->doLater("sensors_tracker", []() {
        auto devices = Singleton<DevicesMap>::i()->ids();
        for (auto id : devices) {
            auto dev = Singleton<DevicesMap>::i()->get(id);
            if (dev == nullptr) {
                continue;
            }

            auto sensor = std::dynamic_pointer_cast<AbstractSensor>(dev);
            if (sensor != nullptr) {
                for (auto &p : sensor->properties())
                {
                    std::string key = sensor->id() + p;
                    auto newValue = sensor->value(p);
                    if (values.find(key) != values.end()) {
                        if(values[key] == newValue) {
                            continue;
                        }

                        values[key] = newValue;
                    }
                    else {
                        values.insert( {key, newValue} );
                    }

                    Singleton<AquaBox>::get()->postUpdate(std::make_shared<ValueChangedEvent>(sensor, p, newValue, "regular_check"));
                }
            }
            auto step = std::dynamic_pointer_cast<AbstractStep>(dev);
            if (step != nullptr) {
                for (auto &p : step->properties())
                {
                    std::string key = step->id() + p;
                    auto newValue = step->steps();
                    if (values.find(key) != values.end()) {
                        if(values[key] == newValue) {
                            continue;
                        }

                        values[key] = newValue;
                    }
                    else {
                        values.insert( {key, newValue} );
                    }

                    Singleton<AquaBox>::get()->postUpdate(std::make_shared<ValueChangedEvent>(step, p, newValue, "regular_check"));
                }
            }
        }
    });
}