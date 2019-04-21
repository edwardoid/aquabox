#ifndef DEVICE_H
#define DEVICE_H

#include <nlohmann/json.hpp>
#include "identifiable.h"
#include "devicetype.h"
#include "property.h"
#include <string>

namespace aquabox
{
    using Metadata = PrimitiveProperty<nlohmann::json, nlohmann::detail::value_t::object>;

    class Device: public ClassProperty
    {
        static const uint64_t INVALID_PRIORITY = static_cast<uint64_t>(-1);
    public:
        Device();
        Device(const Device& src);
        Device(Device&& src);
        const Device& operator = (const Device& src);

        bool turnedOn() const;

        void turnOn();
        void turnOff();

        void addPendingAction(uint64_t priority, ActionType action);
        bool performPendingAction();

        std::vector<std::shared_ptr<class Rule>> rules() const;

        property(Id, StringProperty)
        property(Index, UnsignedNumberProperty)
        property(Pin, UnsignedShortNumberProperty);
        property(Name, StringProperty)
        property(Type, EnumProperty<DeviceType>)
        property(Class, EnumProperty<DeviceClass>)
        property(Meta, Metadata)

    private:
        uint64_t m_pendingActionPriority = INVALID_PRIORITY;
        ActionType m_pendingAction;
    };
}

#endif // DEVICE_H