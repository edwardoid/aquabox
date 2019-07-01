#include "hardware.h"
#include "layer.h"
#include "bus.h"

using namespace aquabox;

std::shared_ptr<Hardware> Hardware::resolve(const std::string& address)
{
    auto l = address.find_first_of(':');
    auto layer = Layer::get(address.substr(0, l++));
    if (layer == nullptr)
        return nullptr;
    
    auto b = address.find_first_of(':', l);
    auto bus = layer->bus(address.substr(l, b - l));
    if (bus == nullptr) {
        return nullptr;
    }
    

    return bus->hw(address.substr(b + 1));
}