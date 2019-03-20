#include "action.h"

using namespace aquabox;

Action::Action(std::string name)
    : ClassProperty(name.c_str(), { &Type, &At }, true)
    , Type("type"), At("at")
{}

Action::Action(const Action& src)
    : Action(src.name)
{
    copyData(src);
    Type = src.Type;
    At = src.At;
}

Action::Action(Action&& src)
    : Action(src.name)
{
    moveData(src);
    Type = std::move(src.Type);
    At = std::move(src.At);
}

const Action& Action::operator = (const Action& src)
{
    copyData(src);
    Type = src.Type;
    At = src.At;
    return *this;
}