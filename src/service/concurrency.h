#ifndef CONCURRENCY_H
#define CONCURRENCY_H

#include "log.h"
#include <thread_pool.hpp>
#include <mutex>
#include <thread>

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
