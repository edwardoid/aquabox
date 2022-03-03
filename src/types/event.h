/*
 * AquaBox
 * Copyright (C) 2019  Eduard Sargsyan
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
        Event();
        void setSender(std::string sender, std::string _class);
        const std::string& sender() const;
        void setData(const nlohmann::json& data);
        void setPropertiesUpdate(const nlohmann::json& update);
        bool hasProperty(const std::string& key) const;
        template<typename T>
        T getProperty(const std::string key)
        {
            return m_properties.at(key).get<T>();
        }
        nlohmann::json serialize() const;

    private:
        std::string m_sender;
        std::string m_class;
        nlohmann::json m_data;
        nlohmann::json m_properties;
    };
} // namespace aquabox
