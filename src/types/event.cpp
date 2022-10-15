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

#include "event.h"

namespace aquabox
{
    void to_json(nlohmann::json& j, const Event& event)
    {
        to_json(j, (const BaseStreamMessage&) event);
        try {
            j.emplace("sender", event.sender());
            j.emplace("class", event.clazz());

            if (event.data().is_object())
                j.emplace("data", event.data());

            if (event.properties().is_object())
                j.emplace("properties", event.properties());
        } catch (const nlohmann::json::exception& ex) {

        }
    }

    void from_json(const nlohmann::json& j, Event& event)
    {
        from_json(j, (BaseStreamMessage&) event);
        try {
            event.setSender(j["sender"], j["class"]);
            if (j.count("data"))
                event.setData(j["data"]);
            if (j.count("properties"))
            event.setProperties(j["properties"]);
        } catch (const nlohmann::json::exception& ex) {

        }
    }
}

using namespace aquabox;

Event::Event()
    : m_properties(nlohmann::json::object())
{
    type = ApiMessageType::Event;
}

void Event::setSender(std::string sender, std::string _class)
{
    m_sender = sender;
    m_class = _class;
}

const std::string& Event::sender() const
{
    return m_sender;
}

const std::string& Event::clazz() const
{
    return m_class;
}

void Event::setPropertiesUpdate(const nlohmann::json& update)
{
    for (auto i : update.items()) {
        m_properties.emplace(i.key(), i.value());
    }
}

void Event::setProperties(const nlohmann::json& properties)
{
    m_properties = properties;
}

bool Event::hasProperty(const std::string& key) const
{
    return m_properties.find(key) != m_properties.end();
}

const nlohmann::json& Event::properties() const
{
    return m_properties;
}