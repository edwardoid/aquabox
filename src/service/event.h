#ifndef EVENT_H
#define EVENT_H

#include <nlohmann/json.hpp>

namespace aquabox
{
    /*
        {
            "sender": "id",
            "class" : "device"
            "data": {
                "any" : "any"
            }
        }
    */
    class Event
    {
    public:
        Event() = default;
        void setSender(std::string sender, std::string _class);
        void setData(const nlohmann::json& data);
        void setPropertiesUpdate(const nlohmann::json& update);
        nlohmann::json serialize() const;
    private:
        std::string m_sender;
        std::string m_class;
        nlohmann::json m_data;
        nlohmann::json m_properties;
    };
}

#endif // EVENT_H