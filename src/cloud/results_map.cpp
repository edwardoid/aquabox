#include "results_map.h"

#include <ctime>
#include <ratio>
#include <chrono>

using namespace aquabox;
//using namespace std::chrono;

void ResultsMap::gotResult(int id)
{
    {
        std::lock_guard<std::mutex> ids(m_ids);
        m_ids.insert(id);
    }
    m_cv.notify_all();

    Lock postponedLock(m_postponedIds);
    for(auto postponedId : m_postponedIds) {
        m_postponedIds.erase(postponedId);
    }
}

bool ResultsMap::waitFor(int id, size_t secondsToWait)
{
    auto t1 = std::chrono::high_resolution_clock::now();
    auto t2 = t1 + secondsToWait * std::chrono::seconds(60);
    std::unique_lock<std::mutex> cvLock(m_cvMx);
    while(t2 > std::chrono::high_resolution_clock::now()) {
        auto waitResult = m_cv.wait_until(cvLock, t2);
        Lock idsLock(m_ids);
        auto pos = m_ids.find(id);
        if (pos != m_ids.end()) {
            m_ids.erase(pos);
            return true;
        }
        if (waitResult == std::cv_status::timeout) {
            Lock postponedLock(m_postponedIds);
            m_postponedIds.insert(id);
            return false;
        }
    }
    Lock postponedLock(m_postponedIds);
    m_postponedIds.insert(id);
    return false;
}