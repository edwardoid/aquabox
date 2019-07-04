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