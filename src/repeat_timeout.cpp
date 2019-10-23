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

#include "repeat_timeout.h"
#include "log.h"

#include <map>

using namespace aquabox;

template<>
Serializable<RepeatUnit>::Enum2StrMap Serializable<RepeatUnit>::val2Str({
    { RepeatUnit::Minute, "minute" },
    { RepeatUnit::Hour, "hour"},
    { RepeatUnit::Day, "day" },
    { RepeatUnit::Week, "week" },
    { RepeatUnit::Month, "month" }
});

template<>
Serializable<RepeatUnit>::Str2EnumMap Serializable<RepeatUnit>::str2Val({
    { "minute", RepeatUnit::Minute },
    { "hour", RepeatUnit::Hour },
    { "day", RepeatUnit::Day },
    { "week", RepeatUnit::Week },
    { "month", RepeatUnit::Month }
});

RepeatTimeout::RepeatTimeout(std::string name)
    : ClassProperty(name.c_str(), { &m_DeleteAfter, &m_Unit }, true)
    , m_DeleteAfter("deleteAfter"), m_Unit("unit")
{}

RepeatTimeout::RepeatTimeout(const RepeatTimeout& src)
    : RepeatTimeout(src.name)
{
    *this = src;
}

RepeatTimeout::RepeatTimeout(RepeatTimeout&& src)
    : RepeatTimeout(src.name)
{
    moveData(src);
    moveProperty(DeleteAfter);
    moveProperty(Unit);
}

const RepeatTimeout& RepeatTimeout::operator = (const RepeatTimeout& src)
{
    copyData(src);
    copyProperty(DeleteAfter);
    copyProperty(Unit);
    return *this;
}