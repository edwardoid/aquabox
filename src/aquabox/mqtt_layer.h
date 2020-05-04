#ifndef MQTT_LAYER_H
#define MQTT_LAYER_H

#include "layer.h"

namespace aquabox
{
    class Mqtt: public Layer
    {
    public:
        std::shared_ptr<Bus> bus(std::string bus) override { return nullptr; }
    };
}

#endif //MQTT_LAYER_H
