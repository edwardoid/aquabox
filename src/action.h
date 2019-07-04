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

        property(At, Time);
        property(Type, EnumProperty<ActionType>);
    };
}

#endif // ACTION_H