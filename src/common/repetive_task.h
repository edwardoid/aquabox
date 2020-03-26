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

#ifndef REPETIVE_TASK_H
#define REPETIVE_TASK_H

#include <chrono>
#include <functional>

namespace aquabox
{
    class RepetiveTask
    {
    public:
        using Resolution = std::chrono::milliseconds;
        using Clock = std::chrono::system_clock;
        RepetiveTask(Resolution timeout,
                     std::function<void(void)> = nullptr);
        void setTask(std::function<void(void)> task);
        void operator() ();
    private:
        Clock::time_point m_last;
        Resolution m_timeout;
        std::function<void(void)> m_task;
    };
}

#endif // REPETIVE_TASK