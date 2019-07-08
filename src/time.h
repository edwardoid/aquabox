#ifndef TIME_H
#define TIME_H

#include "property.h"
#include "time_details.h"

namespace aquabox
{
    class Time: public UnsignedNumberProperty
    {
    public:
        Time(std::string name, bool required = true);
        TimeDetails getDetails() const;
    };
}

#endif // TIME_H