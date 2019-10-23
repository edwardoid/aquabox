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

#ifndef IDENTIFIABLES_MAP_H
#define IDENTIFIABLES_MAP_H

#include <string>
#include <unordered_map>
#include <mutex>
#include <memory>

namespace aquabox
{
    template<typename T>
    
    class Map: public std::unordered_map<std::string /* id */, std::shared_ptr<T>>
             , public std::recursive_mute 
    {
    public:
        using IdType = std::string;
        using DataType = T;
        using DataRef = std::shared_ptr<DataType>;
        bool contains(IdType id);
        bool remove(IdType id);
        bool update(IdType id, DataRef instance);
    };

    template<typename T>
    class LockedMap
    {
    public:
        using ContainerType = Map<T>;
        using IdType = ContainerType::IdType;
        using DataRef = ContainerType::DataRef;

        LockedMap(ContainerType& src);
        LockedMap(LockedMap&& ref);
        
        bool contains(IdType id);
        DataRef get(IdType id);
        DataRef insert(IdType id);

        operator ContainerType& ()
        {

        }

        operator const ContainerType() const
        {

        }
    private:
        ContainerType m_stored;
    };
}

#endif// IDENTIFIABLES_MAP_H