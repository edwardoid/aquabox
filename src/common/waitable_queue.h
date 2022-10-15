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

#include <queue>
#include <mutex>
#include <condition_variable>

namespace aquabox
{
    template<typename T, int max_size = std::numeric_limits<int>::max()>
    class WaitableQueue
    {
    public:
        bool push(const T& elem)
        {
            std::unique_lock<std::mutex> ul(m_mx);
            bool drop = m_elements.size() > max_size;
            if (drop) {
                m_elements.pop();
            }
            m_elements.push(elem);
            m_cv.notify_one();
            return !drop;
        }
        const T front() const
        {
            std::unique_lock<std::mutex> ul(m_mx);
            while(m_elements.empty) {
                m_cv.wait(ul, [this]() { return !m_elements.empty(); });
            }
            return m_elements.front();
        }
        T pop(size_t* pending = nullptr)
        {
            std::unique_lock<std::mutex> ul(m_mx);
            while(m_elements.empty()) {
                m_cv.wait(ul, [this]() { return !m_elements.empty(); });
            }
            T v = m_elements.front();
            m_elements.pop();
            if (pending)
                *pending = m_elements.size();
            return v;
        }
    private:
        std::queue<T> m_elements;
        std::mutex m_mx;
        std::condition_variable m_cv;
    };
}