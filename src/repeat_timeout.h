#ifndef REPEAT_TIMEOUT_H
#define REPEAT_TIMEOUT_H

#include <nlohmann/json.hpp>
#include "property.h"
#include "enumerations.h"
#include "time.h"

namespace aquabox
{
    enum class RepeatUnit: int
    {
        Minute = 0,
        Hour = 1,
        Day = 2,
        Week = 3,
        Month = 4
    };

    class RepeatTimeout: public ClassProperty
    {
    public:
        RepeatTimeout(std::string name);
        RepeatTimeout(const RepeatTimeout& src);
        RepeatTimeout(RepeatTimeout&& src);
        const RepeatTimeout& operator = (const RepeatTimeout& src);

        property(Unit, EnumProperty<RepeatUnit>);
        property(DeleteAfter, Time);
    };
}

#endif // REPEAT_TIMEOU_H