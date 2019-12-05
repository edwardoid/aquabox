#ifndef LOCKABLE_CONTAINER_H
#define LOCKABLE_CONTAINER_H

#include <mutex>
#include <map>
#include <unordered_map>
#include <set>

namespace aquabox
{
    template<class Container,
            class Lock = std::mutex>
    class LockableContainer: public Container, public Lock
    {};

    template<typename T, class Lock = std::mutex>
    struct LockableSet: public LockableContainer<std::set<T>, Lock> {};

    template<typename K, typename V, class Lock = std::mutex>
    struct LockableMap: public LockableContainer<std::map<K, V>, Lock> {};

    template<typename K, typename V, class Lock = std::mutex>
    struct LockableHashMap: public LockableContainer<std::unordered_map<K, V>, Lock> {};
}


#endif // LOCKABLE_CONTAINER_H