#ifndef TIME_DETAILS_H
#define TIME_DETAILS_H

#include <chrono>
#include <string>

namespace aquabox
{
    class TimeDetails
    {
    public:
        TimeDetails(std::time_t when);
        int64_t from (const TimeDetails& src, bool mins = true, bool hours = true, bool day = false, bool month = true) const; 
        int64_t seconds(bool mins, bool hours, bool day, bool month) const;
        std::string to_string() const;
    private:
        std::time_t m_when;
        std::tm m_internal;
    };
}

#endif // TIME_DETAIL_H