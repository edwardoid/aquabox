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

#pragma once

#include <chrono>
#include <condition_variable>
#include <map>
#include <mutex>
#include <set>

namespace aquabox
{
    template<typename T>
    class ResultsMap
    {
    public:
        void ready(size_t id, const T& data)
        {
            {
                std::lock_guard<std::mutex> ready(m_readyMx);
                auto p = m_ready.find(id);
                if (p == m_ready.end()) {
                    return;
                }
                p->second.first = true;
                p->second.second = data;
            }
            m_waitCV.notify_all();
        }

        bool waitFor(size_t id, T& cmd)
        {
            {
                std::lock_guard<std::mutex> ready(m_readyMx);
                m_ready.insert({id, {false, T()}});
            }

            auto endTimepoint = std::chrono::system_clock::now() + std::chrono::seconds(10);
            std::unique_lock<std::mutex> waitLock(m_waitMx);
            while (m_waitCV.wait_until(waitLock, endTimepoint) == std::cv_status::no_timeout) {
                std::lock_guard<std::mutex> ready(m_readyMx);
                if (m_ready[id].first) {
                    cmd = m_ready[id].second;
                    m_ready.erase(id); // no need to wait
                    return true;
                }
            }

            std::lock_guard<std::mutex> ready(m_readyMx);
            m_ready.erase(id);
            return false;
        }

    private:
        std::map<size_t, std::pair<bool, T>> m_ready;
        std::condition_variable m_waitCV;
        std::mutex m_waitMx;
        std::mutex m_readyMx;
    };
} // namespace aquabox
