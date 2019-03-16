#ifndef DEVICE_H
#define DEVICE_H

#include <nlohmann/json.hpp>
#include "identifiable.h"
#include "devicetype.h"

namespace aquabox
{
    class Device: public Identifiable
    {
    public:
        Device() = default;

        bool init(const nlohmann::json& data);
        nlohmann::json data() const;

        void read();
        const std::string& name() const;
        void setName(std::string name);

        DeviceClass deviceClass() const;
        void setClass(DeviceClass deviceClass);
        DeviceType type() const;
        void setType(DeviceType type);
        const nlohmann::json& meta() const;
        void setMeta(const nlohmann::json& meta);
    private:
        std::string m_name;
        DeviceClass m_class;
        DeviceType m_type;
        nlohmann::json m_meta;
    };
}

#endif // DEVICE_H