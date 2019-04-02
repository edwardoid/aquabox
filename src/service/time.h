#ifndef TIME_H
#define TIME_H

#include "property.h"
#include <chrono>
#include <ctime>

namespace aquabox
{
    class Time: public UnsignedNumberProperty
    {
    public:
        Time(std::string name, bool required = true);
        std::tm getDetails() const;
    };
}

#endif // TIME_H