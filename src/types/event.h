/*
 * AquaBox
 * Copyright (C) 2019-2022  Eduard Sargsyan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <nlohmann/json.hpp>
#include <types/base_stream_message.h>
#include <types/serial.h>

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
    class Event: public BaseStreamMessage
    {
    public:
        Event(const std::string& boxId = serial());
        const std::string& boxId() const { return m_boxId; }
        inline void setBoxId(const std::string& boxId) { m_boxId = boxId; }
        void setSender(std::string sender, std::string _class);
        const std::string& sender() const;
        const std::string& clazz() const;
        inline void setData(const nlohmann::json& data) { m_data = data; }
        const nlohmann::json& data() const { return m_data; }
        void setPropertiesUpdate(const nlohmann::json& update);
        void setProperties(const nlohmann::json& update);
        bool hasProperty(const std::string& key) const;
        template<typename T>
        T getProperty(const std::string key)
        {
            return m_properties.at(key).get<T>();
        }
        const nlohmann::json& properties() const;

        static Event cameOnline(const std::string& boxId);
        static Event wentOffline(const std::string& boxId);
    private:
        std::string m_boxId;
        std::string m_sender;
        std::string m_class;
        nlohmann::json m_data;
        nlohmann::json m_properties;
    };

    void to_json(nlohmann::json& j, const Event& req);
    void from_json(const nlohmann::json& j, Event& req);
} // namespace aquabox
