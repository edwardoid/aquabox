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

#include "event.h"

using namespace aquabox;

Event::Event() { m_properties = nlohmann::json({}); }

void Event::setSender(std::string sender, std::string _class)
{
    m_sender = sender;
    m_class = _class;
}

void Event::setData(const nlohmann::json& data)
{
    for (auto i : data.items()) {
        m_data.emplace(i.key(), i.value());
    }
}

void Event::setPropertiesUpdate(const nlohmann::json& update)
{
    for (auto i : update.items()) {
        m_properties.emplace(i.key(), i.value());
    }
}

nlohmann::json Event::serialize() const
{
    return nlohmann::json({ { "sender", m_sender },
        { "class", m_class },
        { "data", m_data },
        { "props", m_properties } });
}