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

#include "action.h"

using namespace aquabox;

Action::Action(std::string name)
    : ClassProperty(name.c_str(), { &m_Type, &m_At }, true)
    , m_Type("type"), m_At("at")
{}

Action::Action(const Action& src)
    : Action(src.name)
{
    *this = src;
}

Action::Action(Action&& src)
    : Action(src.name)
{
    moveData(src);
    moveProperty(Type);
    moveProperty(At)
}

const Action& Action::operator = (const Action& src)
{
    copyData(src);
    copyProperty(Type);
    copyProperty(At);
    return *this;
}