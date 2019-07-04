#include "concurrency.h"
#include "log.h"

using namespace aquabox;

tp::ThreadPool Concurrency::m_pool;

void Concurrency::doLater(Task&& task)
{
    LOG_DEBUG << "Adding new task to global thread pool";
    m_pool.tryPost(task);
}
