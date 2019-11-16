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

#ifndef TIME_DETAILS_H
#define TIME_DETAILS_H

#include <chrono>
#include <string>

namespace aquabox
{
    class TimeDetails
    {
    public:
        TimeDetails(std::time_t when);
        int64_t from (const TimeDetails& src, bool mins = true, bool hours = true, bool day = false, bool month = true) const; 
        int64_t seconds(bool mins, bool hours, bool day, bool month) const;
        std::string to_string() const;
    private:
        std::time_t m_when;
        std::tm m_internal;
    };
}

#endif // TIME_DETAIL_H