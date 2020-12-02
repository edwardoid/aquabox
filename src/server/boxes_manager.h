#ifndef BOXES_MANAGER_H
#define BOXES_MANAGER_H

#include <unordered_map>
#include <mutex>
#include "results_map.h"
#include "command.h"

namespace aquabox
{
    class BoxesManager
    {
    public:
        static std::shared_ptr<class Box> get(std::string id, bool createIfNotExists = false);
        static bool eraseIfNeed(std::string id);
    private:
        static BoxesManager self;

        ResultsMap<Command> m_results;
        std::unordered_map<std::string, std::shared_ptr<class Box>> m_connections;
        std::mutex m_mx;
    };
}

#endif // BOXES_MANAGER_H