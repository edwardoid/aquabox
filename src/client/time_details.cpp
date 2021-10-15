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

#include "time_details.h"

using namespace aquabox;

TimeDetails::TimeDetails(std::time_t when)
    : m_when(when)
{
    m_internal = *std::localtime(&m_when);
}

int64_t TimeDetails::from(const TimeDetails& src, bool mins, bool hours,
    bool day, bool month) const
{
    return src.seconds(mins, hours, day, month) - this->seconds(mins, hours, day, month);
}

int64_t TimeDetails::seconds(bool mins, bool hours, bool day,
    bool month) const
{
    int64_t res = 0;

    res += mins ? m_internal.tm_min * 60 : 0;
    res += hours ? m_internal.tm_hour * 60 * 60 : 0;
    res += day ? m_internal.tm_mday * 24 * 60 * 60 : 0;
    res += month ? m_internal.tm_mon * 31 * 24 * 60 * 60 : 0; //  good enough

    return res;
}

std::string TimeDetails::to_string() const
{
    char str[256];
    sprintf(str, "H = %d M = %d", m_internal.tm_hour, m_internal.tm_min);
    return str;
}