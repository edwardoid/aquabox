#include "device.h"
#include "aquabox.h"
#include "rule.h"
#include "log.h"

using namespace aquabox;

Device::Device()
    : ClassProperty("-class Device", { &m_Id, &m_Index, &m_Name, &m_Type, &m_Class, &m_Meta })
    , m_Id("id"), m_Index("index", false),m_Name("name"), m_Type("type"), m_Class("class"), m_Meta("meta")
{}

Device::Device(const Device& src)
    : Device()
{
    *this = src;
}

Device::Device(Device&& src)
    : Device()
{
    moveData(src);
    moveProperty(Id);
    moveProperty(Index);
    moveProperty(Name);
    moveProperty(Type);
    moveProperty(Class);
    moveProperty(Meta);
}

const Device& Device::operator = (const Device& src)
{
    copyData(src);
    copyProperty(Id);
    moveProperty(Index);
    copyProperty(Name);
    copyProperty(Type);
    copyProperty(Class);
    copyProperty(Meta);

    return *this;
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
