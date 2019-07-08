#ifndef RULE_ACTION_H
#define RULE_ACTION_H

#include "enumerations.h"

namespace aquabox
{
    class Action
    {
    public:
        Action() = default;

        ActionType action() const;
        void setAction(ActionType action);
    private:
        ActionType m_action;
    };
}

#endif // RULE_ACTION_H