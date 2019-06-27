#include "device.h"
#include "aquabox.h"
#include "rule.h"
#include "log.h"
#include "event.h"
#include <time.h>
#include <fstream>

using namespace aquabox;

Device::Device()
    : ClassProperty("-class Device", { &m_Id, &m_Index, &m_Pin, &m_RulesEnabled, &m_Name, &m_Type, &m_Class, &m_Meta })
    , m_Id("id"), m_Index("index", false), m_Pin("pin"), m_RulesEnabled("rulesEnabled")
    , m_Name("name"), m_Type("type"), m_Class("class"), m_Meta("meta")
{}

Device::Device(const Device& src)
    : Device()
{
    *this = src;
}

bool Device::parseFile(const std::string& file)
{
    std::ifstream is(file);

    std::string content, line;
    while(std::getline(is, line)) {
        content += line;
    }

    nlohmann::json root = nlohmann::json::parse(content);

    if(!parse(root)) {
        LOG_ERROR << "Device from " << file << " can not be parsed!";
        return false; 
    }
    return true;
}

bool Device::saveToFile(const std::string& file)
{
    std::ofstream os(file);
    if(!os) {
        LOG_ERROR << "Can't write to " << file << strerror(errno);
        return false;
    }
    nlohmann::json data;
    json(data);
    os << data.dump(4);
    os.flush();
    os.close();
    return true;
}

Device::Device(Device&& src)
    : Device()
{
    moveData(src);
    moveProperty(Id);
    moveProperty(Index);
    moveProperty(Pin);
    moveProperty(RulesEnabled);
    moveProperty(Name);
    moveProperty(Type);
    moveProperty(Class);
    moveProperty(Meta);
}

const Device& Device::operator = (const Device& src)
{
    copyData(src);
    copyProperty(Id);
    copyProperty(Index);
    copyProperty(Pin);
    copyProperty(RulesEnabled);
    copyProperty(Name);
    copyProperty(Type);
    copyProperty(Class);
    copyProperty(Meta);

    return *this;
}

void Device::copyData(const Device& src)
{
    copyProperty(Index);
    copyProperty(RulesEnabled);
    copyProperty(Name);
    copyProperty(Type);
    copyProperty(Class);
    copyProperty(Meta);
}

std::vector<std::shared_ptr<Rule>> Device::rules() const
{
    RulesMap rules = AquaBox::i().rules();
    std::vector<std::shared_ptr<Rule>> res;
    for(auto r : rules) {
        if (r.second->getDevice().Value == m_Id.Value) {
            res.push_back(r.second);
        }
    }

    return res;
}
