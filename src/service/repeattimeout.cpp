#include "repeattimeout.h"
#include "log.h"

#include <map>

using namespace aquabox;

RepeatTimeout::RepeatTimeout(std::string name)
    : ClassProperty(name.c_str(), { &Count }, true)
    , Count("count")
{}

RepeatTimeout::RepeatTimeout(const RepeatTimeout& src)
    : RepeatTimeout(src.name)
{
    copyData(src);
    Count = src.Count;
}

RepeatTimeout::RepeatTimeout(RepeatTimeout&& src)
    : RepeatTimeout(src.name)
{
    moveData(src);
    Count = std::move(src.Count);
}

const RepeatTimeout& RepeatTimeout::operator = (const RepeatTimeout& src)
{
    copyData(src);
    Count = src.Count;
    return *this;
}