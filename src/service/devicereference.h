#ifndef DEVICE_REFERENCE_H
#define DEVICE_REFERENCE_H

#include "property.h"

namespace aquabox
{
    class DeviceReference: public StringProperty
    {
    public:
        DeviceReference(std::string name, bool required = true);
        std::shared_ptr<class Device> resolve() const;
    };
}

#endif // DEVICE_REFERENCE_H