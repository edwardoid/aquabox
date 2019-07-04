#include "hardware.h"
#include "layer.h"
#include "bus.h"
#include "log.h"

using namespace aquabox;

std::shared_ptr<Hardware> Hardware::resolve(const std::string& address)
{
    auto l = address.find_first_of(':');
    auto layer = Layer::get(address.substr(0, l++));
    if (layer == nullptr) {
        LOG_ERROR << "Can't find layer: " << address.substr(0, l - 1);
        return nullptr;
    }
    
    auto b = address.find_first_of(':', l);
    auto bus = layer->bus(address.substr(l, b - l));
    if (bus == nullptr) {
        LOG_ERROR << "Can't find bus: " << address.substr(l, b - l);
        return nullptr;
    }
    

    return bus->hw(address.substr(b + 1));
}