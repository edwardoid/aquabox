#include "jsonwrapper.h"
#include <rapidjson/prettywriter.h>

using namespace aquabox;



template<>
void JsonWrapper::set<std::string>(std::string key, std::string value)
{
    auto pos = FindMember(key.c_str());

    rapidjson::Value v;
    v.SetString(value.c_str(), GetAllocator());

    if (pos == MemberEnd())
        AddMember(rapidjson::Value().SetString(key.c_str(), GetAllocator()), v, GetAllocator());
    else
        pos->value = v;
}

JsonWrapper::JsonWrapper() : rapidjson::Document()
{
    SetObject();
}

std::string JsonWrapper::str(bool pretty) const
{
    rapidjson::StringBuffer buffer;
    buffer.Clear();
    if(pretty) {
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        Accept(writer);
    }
    else {
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        Accept(writer);
    }
    

    return buffer.GetString();
}

JsonWrapper::StringRefType JsonWrapper::ref(std::string& str)
{
    return rapidjson::StringRef(str.c_str());
}
