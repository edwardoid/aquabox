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

#include "repetive_task.h"

using namespace aquabox;

RepetiveTask::RepetiveTask(RepetiveTask::Resolution timeout,
    std::function<void(void)> task)
    : m_timeout(timeout)
    , m_task(task)
    , m_last(Clock::now())
{
}

void RepetiveTask::setTask(std::function<void(void)> task) { m_task = task; }

void RepetiveTask::operator()()
{
    Resolution elapsed = std::chrono::duration_cast<Resolution>(Clock::now() - m_last);
    if (elapsed > m_timeout) {
        m_task();
        m_last = Clock::now();
    }
}