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

#include <mutex>

namespace aquabox
{
    template<class T>
    struct Singleton
    {
        Singleton(const Singleton&) = delete;
        Singleton(Singleton&&) = delete;
        Singleton& operator=(const Singleton&) = delete;
        Singleton();

        static bool available;
        static T* instance;
        static T* get()
        {
            static std::mutex lock;
            std::lock_guard<std::mutex> guard(lock);
            if (!available) {
                return nullptr;
            }
            if (instance == nullptr) {
                instance = create();
            }

            return instance;
        }

        inline static T* i() { return get(); }

        static void destroy()
        {
            if (instance != nullptr)
                delete instance;
            instance = nullptr;
        }

    protected:
        static T* create() { return new T(); }
    };

    template<class T>
    bool Singleton<T>::available = true;

    template<class T>
    T* Singleton<T>::instance = nullptr;
} // namespace aquabox