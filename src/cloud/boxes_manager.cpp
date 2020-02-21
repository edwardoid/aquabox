#include "boxes_manager.h"
#include "box.h"

using namespace aquabox;

BoxesManager BoxesManager::self;

std::shared_ptr<Box> BoxesManager::get(std::string id, bool createIfNotExists)
{
    std::lock_guard<std::mutex> ready(self.m_mx);

    auto p = self.m_connections.find(id);
    auto box = p == self.m_connections.end() ? nullptr : p->second;

    if (box == nullptr) {
        box = std::make_shared<Box>(self.m_results);
        self.m_connections.insert({id, box});
    }

    return box;
}

bool BoxesManager::eraseIfNeed(std::string id)
{
    std::lock_guard<std::mutex> ready(self.m_mx);
    auto p = self.m_connections.find(id);
    if (p != self.m_connections.end() && p->second->empty()) {
        self.m_connections.erase(p);
        return true;
    }

    return false;
}
