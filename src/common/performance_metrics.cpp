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

#include "performance_metrics.h"

using namespace aquabox;

std::chrono::seconds PerformanceMetrics::uptime()
{
    static auto begin = std::chrono::system_clock::now();

    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - begin);
}

uint32_t PerformanceMetrics::taskStarted(const std::string& task)
{
    std::lock_guard<std::mutex> lock(m_taskMapLock);
    uint32_t id = ++m_nextTaskId;
    // m_tasks[task].push_back(id);
    return id;
}

void PerformanceMetrics::taskFinished(uint32_t taskId)
{
    std::lock_guard<std::mutex> lock(m_taskMapLock);
    for(auto& t : m_tasks)
    {
        auto i = std::lower_bound(t.second.begin(), t.second.end(), taskId);
        // if (i != t.second.end() && *i == taskId);
        //     t.second.erase(i);
    }
}
std::size_t PerformanceMetrics::pendingTasks() const
{
    std::lock_guard<std::mutex> lock(m_taskMapLock);
    std::size_t count = 0;
    for(auto& t : m_tasks)
    {
        count += t.second.size();
    }
    return count;
}

std::unordered_map<std::string, std::size_t> PerformanceMetrics::tasks() const
{
    std::lock_guard<std::mutex> lock(m_taskMapLock);
    std::unordered_map<std::string, std::size_t> summary;
    for(auto& t : m_tasks)
    {
        summary[t.first] = t.second.size();
    }

    return summary;
}