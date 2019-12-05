#ifndef RESULTS_MAP
#define RESULTS_MAP

#include <time.h>
#include "lockable_container.h"
#include <condition_variable>
#include <mutex>
#include <set>

namespace aquabox
{
    class ResultsMap
    {
    public:
        void gotResult(int id);
        bool waitFor(int id, size_t secondsToWait);
    private:
        using Lock = std::lock_guard<LockableSet<int>>;
        LockableSet<int> m_ids;
        LockableSet<int> m_postponedIds;

        std::mutex m_cvMx;
        std::condition_variable m_cv;
    };
}

#endif // RESULTS_MAP