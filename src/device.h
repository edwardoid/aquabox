#ifndef DEVICE_H
#define DEVICE_H

#include <nlohmann/json.hpp>
#include "identifiable.h"
#include "devicetype.h"
#include "property.h"
#include "hardware.h"
#include <string>

namespace aquabox
{
    using Metadata = PrimitiveProperty<nlohmann::json, nlohmann::detail::value_t::object>;

    class Device: public ClassProperty
    {
    public:
        Device();
        Device(const Device& src);
        Device(Device&& src);
        const Device& operator = (const Device& src);
        void copyData(const Device& src);

        bool parseFile(const std::string& file);
        bool saveToFile(const std::string& file);

        std::vector<std::shared_ptr<class Rule>> rules() const;

        Bytes read(uint32_t len) const;

        property(Id, StringProperty)
        property(Index, UnsignedNumberProperty)
        property(Address, StringProperty);
        property(RulesEnabled, BooleanProperty)
        property(Name, StringProperty)
        property(Type, EnumProperty<DeviceType>)
        property(Class, EnumProperty<DeviceClass>)
        property(Meta, Metadata)
    };
}

#endif // DEVICE_H