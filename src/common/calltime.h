/*
 * AquaBox
 * Copyright (C) 2020  Eduard Sargsyan
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

#include <common/log.h>
#include <chrono>

#define CONCAT(a, b) CONCAT_INNER(a, b)
#define CONCAT_INNER(a, b) a ## b

#define UNIQUE_NAME(base) CONCAT(base, __COUNTER__)

#define MEASURE(D) aquabox::Calltime UNIQUE_NAME(ct) (D);
#define MEASURE_FN MEASURE(__FUNCTION__)


namespace aquabox
{
    class Calltime
    {
    public:
        Calltime(const char* description)
            : m_description(description)
            , m_start(std::chrono::high_resolution_clock::now())
            {}
        ~Calltime()
        {
            std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_start);
            LOG_ERROR << m_description << " duration "  << duration.count() << "ms.";
        }
    private:
        const char* m_description;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
    };
}