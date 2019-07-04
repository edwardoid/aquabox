#ifndef LAYER_H
#define LAYER_H

#include "bus.h"
#include <string>
#include <memory>

namespace aquabox
{
    class Layer
    {
    public:
        static std::shared_ptr<Layer> get(const std::string layer);
        virtual std::shared_ptr<Bus> bus(const std::string bus) = 0;
    };
}

#endif // LAYER_H