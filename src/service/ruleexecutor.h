#ifndef RULE_EXECUTOR_H
#define RULE_EXECUTOR_H

#include "timer.h"
#include "rule.h"
#include <memory>

namespace aquabox
{
    class RuleExecutor
    {
    public:
        RuleExecutor(std::shared_ptr<class Rule>& rule, Timer::ClockTimePoint now);
        RuleExecutor(const RuleExecutor&) = default;
        RuleExecutor(RuleExecutor&& src) = default;
        void operator() ();
    private:
        void turnOff();
        void turnOn();
    private:
        const Timer::ClockTimePoint m_now;
        std::shared_ptr<class Rule> m_rule;
    };
}

#endif // RULE_EXECUTOR_H