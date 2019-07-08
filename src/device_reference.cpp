#include "device_reference.h"
#include "aquabox.h"
#include "device.h"

using namespace aquabox;

DeviceReference::DeviceReference(std::string name, bool required)
    : StringProperty(name.c_str(), required)
{}

std::shared_ptr<Device> DeviceReference::resolve() const
{
    auto i = AquaBox::i().devices().find(Value);
    return i == AquaBox::i().devices().end() ? nullptr : i->second;
}