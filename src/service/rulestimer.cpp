#include "rulestimer.h"
#include "rule.h"
#include "device.h"
#include "ruleexecutor.h"
#include "aquabox.h"
#include "event.h"
#include "updatevents.h"
#include <set>
#include <plog/Log.h>
#include <plog/Severity.h>
using namespace aquabox;

RulesTimer::RulesTimer()
    : m_timer("RulesTimer")
    , m_rulesAppender("rules.log")
    , m_rulesLogger(plog::Severity::debug)
{
    m_rulesLogger.addAppender(&m_rulesAppender);
}

void RulesTimer::start()
{
    m_timer.start([this](const Timer::ClockTimePoint& when){
        AquaBox::i().doLater([this, when](){
            RulesMap rulesToDelete;
            for(auto rule : AquaBox::i().rules())
            {
                const Time at = rule.second->getRepeat().getDeleteAfter();
                const RepeatUnit unit = rule.second->getRepeat().getUnit();
                if(at.Value != 0 &&
                   at.getDetails().from(TimeDetails(Timer::Clock::to_time_t(when)),
                                        true, true, true, true) >=0)
                {
                    rulesToDelete.insert({ rule.first, rule.second });
                }
                else if(rule.second->getEnabled().Value)
                {
                    RuleExecutor re(rule.second, when);
                    re();
                }
            }

            for(auto rule : rulesToDelete)
            {
                AquaBox::i().removeRule(rule.second);
            }

            for(auto& dev : AquaBox::i().devices())
            {
                bool isOn = dev.second->turnedOn();
                if(dev.second->performPendingAction()) {
                    LOG_DEBUG_IF_(1, true) << "Device" << dev.second->getName().Value << " changed to Turned " << (dev.second->turnedOn() ? "ON" : "OFF");
                }
                if (isOn != dev.second->turnedOn()) {
                    AquaBox::i().postUpdate(std::make_shared<RuleTriggeredEvent>(dev.second));
                }
            }
        });
    }, std::chrono::milliseconds(1000)); // once per second
}

void RulesTimer::stop()
{
    m_timer.stop();
}