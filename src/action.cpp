#include "action.h"

using namespace aquabox;

Action::Action(std::string name)
    : ClassProperty(name.c_str(), { &m_Type, &m_At }, true)
    , m_Type("type"), m_At("at")
{}

Action::Action(const Action& src)
    : Action(src.name)
{
    *this = src;
}

Action::Action(Action&& src)
    : Action(src.name)
{
    moveData(src);
    moveProperty(Type);
    moveProperty(At)
}

const Action& Action::operator = (const Action& src)
{
    copyData(src);
    copyProperty(Type);
    copyProperty(At);
    return *this;
}