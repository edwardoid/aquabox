#ifndef TIME_H
#define TIME_H

#include "property.h"

namespace aquabox
{
    class Time: public UnsignedNumberProperty
    {
    public:
        Time(std::string name, bool required = true);
    };
}

#endif // TIME_H