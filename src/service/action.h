#ifndef ACTION_H
#define ACTION_H

#include "enumerations.h"
#include "property.h"
#include "time.h"

namespace aquabox
{
    class Action: public ClassProperty
    {
    public:
        Action(std::string name = "");
        Action(const Action& src);
        Action(Action&& src);
        const Action& operator = (const Action& src);

        EnumProperty<ActionType> Type;
        Time At;
    };
}

#endif // ACTION_H