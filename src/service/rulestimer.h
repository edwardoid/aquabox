#ifndef RULES_TIMER_H
#define RULES_TIMER_H

#include "timer.h"

namespace aquabox
{
    class RulesTimer
    {
    public:
        RulesTimer();
        void start();
        void stop();
    private:
        Timer m_timer;
    };
}

#endif // RULES_TIMER_H