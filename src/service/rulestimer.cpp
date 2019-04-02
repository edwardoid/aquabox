#include "rulestimer.h"
#include "rule.h"
#include "ruleexecutor.h"
#include "aquabox.h"
using namespace aquabox;

RulesTimer::RulesTimer()
    : m_timer("RulesTimer")
{
}

void RulesTimer::start()
{
    m_timer.start([](const Timer::ClockTimePoint& when){
        for(auto rule : AquaBox::i().rules())
        {
            AquaBox::i().doLater(RuleExecutor(rule.second, when));
        }
    }, std::chrono::milliseconds(1000)); // once per second
}

void RulesTimer::stop()
{
    m_timer.stop();
}