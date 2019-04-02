#include "repeattimeout.h"
#include "log.h"

#include <map>

using namespace aquabox;

template<>
Serializable<RepeatUnit>::Enum2StrMap Serializable<RepeatUnit>::val2Str({
    { RepeatUnit::Minute, "minute" },
    { RepeatUnit::Hour, "hour"},
    { RepeatUnit::Day, "day" },
    { RepeatUnit::Week, "week" },
    { RepeatUnit::Month, "month" }
});

template<>
Serializable<RepeatUnit>::Str2EnumMap Serializable<RepeatUnit>::str2Val({
    { "minute", RepeatUnit::Minute },
    { "hour", RepeatUnit::Hour },
    { "day", RepeatUnit::Day },
    { "week", RepeatUnit::Week },
    { "month", RepeatUnit::Month }
});

RepeatTimeout::RepeatTimeout(std::string name)
    : ClassProperty(name.c_str(), { &m_Count, &m_Unit }, true)
    , m_Count("count"), m_Unit("unit")
{}

RepeatTimeout::RepeatTimeout(const RepeatTimeout& src)
    : RepeatTimeout(src.name)
{
    *this = src;
}

RepeatTimeout::RepeatTimeout(RepeatTimeout&& src)
    : RepeatTimeout(src.name)
{
    moveData(src);
    moveProperty(Count);
    moveProperty(Unit);
}

const RepeatTimeout& RepeatTimeout::operator = (const RepeatTimeout& src)
{
    copyData(src);
    copyProperty(Count);
    copyProperty(Unit);
    return *this;
}