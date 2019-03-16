#include "device.h"
#include "log.h"

using namespace aquabox;

aquabox::DeviceClass classFromString(std::string str)
{
    using namespace aquabox;
    if (str == "relay") return DeviceClass::Relay;
    if (str == "analog_relay") return DeviceClass::AnalogRelay;

    return DeviceClass::Unknown;
}

static std::string to_string(const aquabox::DeviceClass& dc)
{
    using namespace aquabox;

    switch (dc)
    {
        case DeviceClass::Relay: return "relay";
        case DeviceClass::AnalogRelay: return "analog_relay"; 
    
        case DeviceClass::Unknown:
        default: return "unknown";
    }

    return "";
}

static aquabox::DeviceType typeFromString(std::string str)
{
    using namespace aquabox;
    if (str == "light") return DeviceType::Light;
    if (str == "compressor") return DeviceType::Compressor;
    if (str == "filter") return DeviceType::Filter;
    if (str == "heater") return DeviceType::Heater;
    if (str == "co2") return DeviceType::CO2;

    return DeviceType::Generic;
}

static std::string to_string(const aquabox::DeviceType& dc)
{
    using namespace aquabox;

    switch (dc)
    {
        case DeviceType::Light: return "light";
        case DeviceType::Compressor: return "compressor";
        case DeviceType::Filter: return "filter";
        case DeviceType::Heater: return "heater";
        case DeviceType::CO2: return "co2";

        case DeviceType::Generic:
        default: return "generic";
    }

    return "";
}

bool Device::init(const nlohmann::json& data)
{
    if (!data.is_object())
    {
        LOG_ERROR << "Can't initialize device from JSON data. Data MUST be an object";
        return false;
    }
    
    auto pos = data.find("id");
    if (pos == data.end()) {
        LOG_ERROR << "Can't find property 'id' in JSON data for device";
        return false;
    }
    if (!pos->is_string()) {
        LOG_ERROR << "Id of device must be a string";
        return false;
    }

    setId(pos->get<std::string>());

    if (id().empty()) {
        LOG_ERROR << "Id can't be empty!";
        return false;
    }

    pos = data.find("name");
    if (pos == data.end()) {
        LOG_ERROR << "Can't find name for device " << id();
        return false;
    }

    if (!pos->is_string()) {
        LOG_ERROR << "Name for device " << id() << " is not a string";
        return false;
    }

    m_name = pos->get<std::string>();

    if (m_name.empty()) {
        LOG_ERROR << "Name for device " << id() << " can't be empty";
        return false;
    }

    pos = data.find("class");
    if (pos == data.end()) {
        LOG_ERROR << "No class information for device " << id();
        return false;
    }

    if (!pos->is_string()) {
        LOG_ERROR << "Device class for " << id() << " is not a string";
        return false;
    }

    m_class = classFromString(pos->get<std::string>());
    LOG_WARNING_IF(m_class == DeviceClass::Unknown) << "Device class of " << id() << " is unknown!";

    pos = data.find("type");
    if (pos == data.end()) {
        LOG_ERROR << "No type information for device " << id();
        return false;
    }

    if (!pos->is_string()) {
        LOG_ERROR << "Device type for " << id() << " is not a string";
        return false;
    }

    m_type = typeFromString(pos->get<std::string>());
    LOG_WARNING_IF(m_type == DeviceType::Generic) << "Device class of " << id() << " is generic!";


    pos = data.find("meta");
    if (pos == data.end()) {
        LOG_DEBUG << "No meta info for device " << id();
        return true;
    }

    if (!pos->is_object()) {
        LOG_ERROR << "Meta info for device " << id() << " is not an object";
        return false;
    }

    m_meta = pos.value();
    return true;
}

nlohmann::json Device::data() const
{
    return nlohmann::json({
        { "id" , id()  },
        { "name", m_name },
        { "class", to_string(m_class) },
        { "type" , to_string(m_type) },
        { "meta" , m_meta }
    });
}

const std::string& Device::name() const
{
    return m_name;
}

void Device::setName(std::string name)
{
    m_name = name;
}

DeviceClass Device::deviceClass() const
{
    return m_class;
}

void Device::setClass(DeviceClass deviceClass)
{
    m_class = deviceClass;
}

DeviceType Device::type() const
{
    return m_type;
}

void Device::setType(DeviceType type)
{
    m_type = type;
}

const nlohmann::json& Device::meta() const
{
    return m_meta;
}

void Device::setMeta(const nlohmann::json& meta)
{
    m_meta = meta;
}
