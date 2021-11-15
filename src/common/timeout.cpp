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

#include "timeout.h"

using namespace aquabox;

Timeout::Timeout(std::chrono::milliseconds timeout)
    : m_timeout(timeout)
{
    reset();
}

void Timeout::setTimeout(std::chrono::milliseconds timeout)
{
    m_timeout = timeout;
}

const std::chrono::milliseconds Timeout::timeout() const { return m_timeout; }

size_t Timeout::expired() const
{
    std::chrono::milliseconds delta = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - m_lastPoint);
    return delta.count() / m_timeout.count();
}

void Timeout::reset()
{
    m_lastPoint = std::chrono::high_resolution_clock::now();
}