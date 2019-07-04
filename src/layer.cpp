#include "layer.h"
#include "physicallayer.h"
#include <unordered_map>

using namespace aquabox;

std::shared_ptr<Layer> Layer::get(std::string layer)
{
    static std::unordered_map<std::string, std::shared_ptr<Layer>> availableLayers({
        { "PHY", std::make_shared<Physical>() }
    });

    auto pos = availableLayers.find(layer);
    return pos == availableLayers.end() ? nullptr : pos->second;
}