#include "rule.h"
#include "log.h"
#include <fstream>

using namespace aquabox;

Rule::Rule()
    : ClassProperty("-class Rule", {&Id, &Device, &CreatedAt, &Repeat, &Actions} , false)
    , Id("id")
    , Device("device")
    , CreatedAt("created_at")
    , Repeat("repeat")
    , Actions("actions")
{}

Rule::Rule(const Rule& src)
    : Rule()
{
    copyData(src);
    Id = src.Id;
    Device = src.Device;
    CreatedAt = src.CreatedAt;
    Repeat = src.Repeat;
    Actions = src.Actions;
}

Rule::Rule(Rule&& src)
    : Rule()
{
    moveData(src);
    Id = std::move(src.Id);
    Device = std::move(src.Device);
    CreatedAt = std::move(src.CreatedAt);
    Repeat = std::move(src.Repeat);
    Actions = std::move(src.Actions);
}
        
const Rule& Rule::operator = (const Rule& src)
{
    copyData(src);
    Id = src.Id;
    Device = src.Device;
    CreatedAt = src.CreatedAt;
    Repeat = src.Repeat;
    Actions = src.Actions;
    return *this;
}


bool Rule::parseFile(const std::string& file)
{
    std::ifstream is(file);

    std::string content, line;
    while(std::getline(is, line))
    {
        content += line;
    }

    nlohmann::json root = nlohmann::json::parse(content);

    if(!parse(root))
    {
        LOG_ERROR << "Rule from " << file << " can not be parsed!";
        return false; 
    }

    if(nullptr == Device.resolve())
    {
        LOG_ERROR << "Device " << Device << " can not be found";
        return false;
    }
    return true;
}