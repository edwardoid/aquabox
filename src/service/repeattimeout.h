#ifndef REPEAT_TIMEOUT_H
#define REPEAT_TIMEOUT_H

#include <nlohmann/json.hpp>
#include "property.h"

namespace aquabox
{
    enum class RepeatUnit: int
    {
        Minute,
        Hour,
        Day,
        Week,
        Month,
        Always
    };

    class RepeatTimeout: public ClassProperty
    {
    public:
        RepeatTimeout(std::string name);
        RepeatTimeout(const RepeatTimeout& src);
        RepeatTimeout(RepeatTimeout&& src);
        const RepeatTimeout& operator = (const RepeatTimeout& src);

        UnsignedNumberProperty Count;
    };
}

#endif // REPEAT_TIMEOU_H