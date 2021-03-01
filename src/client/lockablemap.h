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

#include <unordered_map>
#include <map>
#include <set>
#include <mutex>
#include <functional>

namespace aquabox
{
    template <typename KeyT, typename ValueT, class Container = std::map<KeyT, ValueT>, class Lock = std::recursive_mutex>
    class LockableAssociativeContainer : public Lock
    {
    public:
        std::set<KeyT> keys() const
        {
            LockableAssociativeContainer* l = const_cast<LockableAssociativeContainer*>(this);
            std::lock_guard<Lock> lock(*l);
            std::set<KeyT> k;
            for (const auto &i : m_container)
            {
                k.insert(i.first);
            }
            return k;
        }
        const ValueT &operator[](const KeyT &key)
        {
            std::lock_guard<Lock> lock(*this);
            return m_container[key];
        }

        ValueT get(const KeyT& key, const ValueT& defaultValue = ValueT())
        {
            std::lock_guard<Lock> lock(*this);
            auto i = m_container.find(key);
            if (i != m_container.end()) {
                return i->second;
            }
            return defaultValue;
        }

        ValueT &at(const KeyT &key) const
        {
            LockableAssociativeContainer* l = const_cast<LockableAssociativeContainer*>(this);
            std::lock_guard<Lock> lock(*l);
            return m_container.at(key);
        }
        bool has(const KeyT &key) const
        {
            LockableAssociativeContainer* l = const_cast<LockableAssociativeContainer*>(this);
            std::lock_guard<Lock> lock(*l);
            return m_container.find(key) != m_container.cend();
        }
        bool has(std::function<bool(const ValueT&)> filter) const
        {
            LockableAssociativeContainer* l = const_cast<LockableAssociativeContainer*>(this);
            std::lock_guard<Lock> lock(*l);
            for (auto &it : m_container)
            {
                if(filter(it.second)) {
                    return true;
                }
            }
            return false;
        }
        void for_each(std::function<void(const KeyT &, ValueT &)> cb)
        {
            std::lock_guard<Lock> lock(*this);
            for (auto &it : m_container)
            {
                cb(it.first, it.second);
            }
        }

        LockableAssociativeContainer operator = (const Container& src)
        {
            set(src);
            return *this;
        }

        void set(const Container& src)
        {
            std::lock_guard<Lock> lock(*this);
            m_container = src;
        }

        void insert(const KeyT &key, const ValueT &value)
        {
            std::lock_guard<Lock> lock(*this);
            m_container[key] = value;
        }

        void for_each(std::function<void(const KeyT &, const ValueT &)> cb) const
        {
            std::lock_guard<Lock> lock(*this);
            for (auto &it : m_container)
            {
                cb(it.first, it.second);
            }
        }

        bool erase(const KeyT& key)
        {
            std::lock_guard<Lock> lock(*this);
            auto i = m_container.find(key);
            if (i == m_container.end()) {
                return false;
            }
            m_container.erase(i);
            return true;
        }

        bool on(const KeyT& key, std::function<void(const KeyT& key, ValueT& value)> cb)
        {
            std::lock_guard<Lock> lock(*this);
            auto i = m_container.find(key);
            if (i == m_container.end()) {
                return false;
            }
            cb(i->first, i->second);
            return true;
        }

        template<typename RetType>
        bool on(const KeyT& key, std::function<RetType (const KeyT& key, ValueT& value)> cb, RetType& ret)
        {
            std::lock_guard<Lock> lock(*this);
            auto i = m_container.find(key);
            if (i == m_container.end()) {
                return false;
            }
            ret = cb(i->first, i->second);
            return true;
        }

        bool empty() const {
            LockableAssociativeContainer* l = const_cast<LockableAssociativeContainer*>(this);
            std::lock_guard<Lock> lock(*l);
            return m_container.empty();
        }

        std::size_t size() const {
            LockableAssociativeContainer* l = const_cast<LockableAssociativeContainer*>(this);
            std::lock_guard<Lock> lock(*l);
            return m_container.size();
        }

    private:
        Container m_container;
    };

    template <typename KeyT, typename ValueT>
    struct LocableMap : public LockableAssociativeContainer < KeyT, ValueT, std::map<KeyT, ValueT> >
    {};

    template <typename KeyT, typename ValueT>
    struct LockableHashMap : public LockableAssociativeContainer < KeyT, ValueT, std::unordered_map<KeyT, ValueT> >
    {};
} // namespace aquabox
