#ifndef JSONWRAPPER_H
#define JSONWRAPPER_H

#define RAPIDJSON_HAS_STDSTRING 1

#include <string>
#include <rapidjson/document.h>


namespace aquabox
{
    class JsonWrapper: public rapidjson::Document
    {
        public:
        JsonWrapper();
        std::string str(bool pretty = true) const;
        template<typename T>
        void set(std::string key, T value);
        static JsonWrapper::StringRefType ref(std::string& str);
    };

    template<typename T>
    void JsonWrapper::set(std::string key, T value)
    {
        auto pos = FindMember(key.c_str());
        if (pos == MemberEnd())
            AddMember(rapidjson::Value().SetString(key.c_str(), GetAllocator()), value, GetAllocator());
        else
            pos->value.Set(value, GetAllocator());
    }

    template<>
    void JsonWrapper::set<std::string>(std::string key, std::string value);
}

#endif // JSONWRAPPER_H