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

#include <chrono>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace aquabox {
class PerformanceMetrics {
public:
    static std::chrono::seconds uptime();
    uint32_t taskStarted(const std::string& task);
    void taskFinished(uint32_t taskId);
    std::size_t pendingTasks() const;
    std::unordered_map<std::string, std::size_t> tasks() const;

private:
    std::unordered_map<std::string, std::vector<uint32_t>> m_tasks;
    uint32_t m_nextTaskId = 0;
    mutable std::mutex m_taskMapLock;
};
}