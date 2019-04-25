#include "event.h"

using namespace aquabox;

Event::Event()
{
    m_properties = nlohmann::json({});
}

void Event::setSender(std::string sender, std::string _class)
{
    m_sender = sender;
    m_class = _class;
}

void Event::setData(const nlohmann::json& data)
{
    for(auto i : data.items())
    {
        m_data.emplace(i.key(), i.value());
    }
}


void Event::setPropertiesUpdate(const nlohmann::json& update)
{
    for(auto i : update.items())
    {
        m_properties.emplace(i.key(), i.value());
    }
}

nlohmann::json Event::serialize() const
{
    return nlohmann::json({
        { "sender", m_sender },
        { "class", m_class },
        { "data", m_data },
        { "props", m_properties } 
    });
}