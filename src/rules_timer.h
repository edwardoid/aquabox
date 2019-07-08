#ifndef RULES_TIMER_H
#define RULES_TIMER_H

#include "timer.h"
#include "log.h"

namespace aquabox
{
    class RulesTimer
    {
    public:
        RulesTimer();
        void start();
        void stop();
    private:
        plog::RollingFileAppender<plog::CsvFormatter> m_rulesAppender;
        plog::Logger<1> m_rulesLogger;

        Timer m_timer;
    };
}

#endif // RULES_TIMER_H