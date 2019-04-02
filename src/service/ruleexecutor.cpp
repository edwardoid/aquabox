#include "ruleexecutor.h"
#include "rule.h"
#include "timer.h"
#include "log.h"
#include "enumerations.h"
#include <ctime>

using namespace aquabox;

RuleExecutor::RuleExecutor(std::shared_ptr<Rule>& rule, Timer::ClockTimePoint now)
    : m_now(now), m_rule(rule)
{}

void RuleExecutor::operator() ()
{
    LOG_INFO << "Processing rule \"" << m_rule->getId() << "\" on " << m_rule->getDevice().Value;
    const std::time_t now = Timer::Clock::to_time_t(m_now);
    const std::tm* nowDetails = std::gmtime(&now);

    const RepeatUnit unit = m_rule->getRepeat().getUnit();
    
    for(const Action& act : *(m_rule->getActions()))
    {
        bool ok = true;
        const std::tm moment = act.getAt().getDetails();

        switch (unit) {
            case RepeatUnit::Minute: {
                ok = moment.tm_min == nowDetails->tm_min;
                break;  
            }
            case RepeatUnit::Hour: {
                ok = moment.tm_min == nowDetails->tm_min &&
                     moment.tm_hour == nowDetails->tm_hour;
                break;
            }
            case RepeatUnit::Day: {
                ok = moment.tm_mday == nowDetails->tm_mday &&
                     moment.tm_hour == nowDetails->tm_hour &&
                     moment.tm_min == nowDetails->tm_min;
                break;
            }
            case RepeatUnit::Week: {
                ok = moment.tm_wday == nowDetails->tm_wday &&
                     moment.tm_hour == nowDetails->tm_hour &&
                     moment.tm_min == nowDetails->tm_min;
                break;
            }
            case RepeatUnit::Month: {
                ok = moment.tm_mon == nowDetails->tm_mon;
                     moment.tm_mday == nowDetails->tm_mday &&
                     moment.tm_hour == nowDetails->tm_hour &&
                     moment.tm_min == nowDetails->tm_min;
                break;
            }
        }
        
        if (ok) {

            switch(act.getType().Value) {
                case ActionType::TurnOff: {
                    return turnOff();
                }
                case ActionType::TurnOn: {
                    return turnOn();
                }
                default: {
                    LOG_WARNING << "Unsupported operation action type " << Serializable<ActionType>::convert(act.getType().Value);
                }
            }
        }
    }
}

void RuleExecutor::turnOn()
{

} 

void RuleExecutor::turnOff()
{
}