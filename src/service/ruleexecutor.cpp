#include "ruleexecutor.h"
#include "rule.h"
#include "timer.h"
#include "log.h"
#include "enumerations.h"
#include "device.h"
#include "timedetails.h"
#include <ctime>

using namespace aquabox;

RuleExecutor::RuleExecutor(std::shared_ptr<Rule>& rule, Timer::ClockTimePoint now)
    : m_now(now), m_rule(rule)
{}

void RuleExecutor::operator() ()
{
    LOG_INFO << "Processing rule \"" << m_rule->getId() << "\" on " << m_rule->getDevice().Value;
    const TimeDetails now(Timer::Clock::to_time_t(m_now));

    const RepeatUnit unit = m_rule->getRepeat().getUnit();
    auto dev = m_rule->getDevice().resolve();
    
    for(const Action& act : *(m_rule->getActions()))
    {
        const TimeDetails at = act.getAt().getDetails();

        int64_t priority = at.from(now, true, unit >= RepeatUnit::Hour, unit >= RepeatUnit::Day, unit >= RepeatUnit::Month);
        
        if (priority >= 0) {
            LOG_DEBUG << "Triggering action " << Serializable<ActionType>::convert(act.getType().Value) << " for " << at.to_string();
            dev->addPendingAction(priority, act.getType().Value);
        }
    }
}