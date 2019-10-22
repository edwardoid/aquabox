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

#ifndef CONCURRENCY_H
#define CONCURRENCY_H

#include "log.h"
#include <thread_pool.hpp>
#include <mutex>
#include <thread>
#include <functional>

namespace aquabox
{

    using Task = std::function<void(void)>;

    class Concurrency
    {
        private:
        Concurrency() = delete;
        Concurrency(Concurrency&&) = delete;
        Concurrency(const Concurrency&) = delete;
        const Concurrency& operator = (const Concurrency) = delete;
        public:
            void doLater(Task&& task);
        private:
            static tp::ThreadPool m_pool; 
    };
}

#endif // CONCURRENCY_H
