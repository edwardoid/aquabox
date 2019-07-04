#include "timedetails.h"

using namespace aquabox;

TimeDetails::TimeDetails(std::time_t when)
    : m_when(when)
{
    m_internal = *std::localtime(&m_when);
}

int64_t TimeDetails::from(const TimeDetails& src, bool mins, bool hours, bool day, bool month) const
{
    return src.seconds(mins, hours, day, month) - this->seconds(mins, hours, day, month);
}

int64_t TimeDetails::seconds(bool mins, bool hours, bool day, bool month) const
{
    int64_t res = 0;

    res += mins ? m_internal.tm_min * 60 : 0;
    res += hours ? m_internal.tm_hour * 60 * 60 : 0;
    res += day ? m_internal.tm_mday * 24 * 60 * 60 : 0;
    res += month ? m_internal.tm_mon * 31 * 24 * 60 * 60 : 0; //  good enough

    return res;
}

std::string TimeDetails::to_string() const
{
    char str[256];
    sprintf(str, "H = %d M = %d", m_internal.tm_hour, m_internal.tm_min);
    return str;
}