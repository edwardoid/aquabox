#include "sensors_tracker.h"
#include "sensor.h"
#include "aquabox.h"
#include "update_events.h"
#include <unordered_map>

using namespace aquabox;

void SensorsTracker::processUpdates() {
    static std::unordered_map<std::string, double> values;

    AquaBox::i().doLater([]() {
        for (auto dev : AquaBox::i().devices()) {
            if (!dev.second->isSensor()) {
                continue;
            }

            auto sensor = std::dynamic_pointer_cast<Sensor>(dev.second);
            if (sensor == nullptr) {
                continue;
            }
            auto newValue = sensor->value();
            if (values.find(sensor->getId()) != values.end()) {
                if(values[sensor->getId()] == newValue) {
                    continue;
                }

                values[sensor->getId()] = newValue;
            }
            else {
                values.insert( {sensor->getId(), newValue} );
            }

            AquaBox::i().postUpdate(std::make_shared<SensorChangedEvent>(sensor, "regular_check"));
        }
    });
}