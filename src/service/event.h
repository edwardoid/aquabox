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
        nlohmann::json serialize() const;
    private:
        std::string m_sender;
        std::string m_class;
        nlohmann::json m_data;
    };
}

#endif // EVENT_H