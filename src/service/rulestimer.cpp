#include "rulestimer.h"
#include "rule.h"
#include "device.h"
#include "ruleexecutor.h"
#include "aquabox.h"
#include "event.h"
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
            for(auto rule : AquaBox::i().rules())
            {
                RuleExecutor re(rule.second, when);
                re();
            }

            for(auto& dev : AquaBox::i().devices())
            {
                if(dev.second->performPendingAction()) {
                    LOG_DEBUG_IF_(1, true) << "Device" << dev.second->getName().Value << " changed to Turned " << (dev.second->turnedOn() ? "ON" : "OFF");
                }
            }
        });
    }, std::chrono::milliseconds(1000)); // once per second
}

void RulesTimer::stop()
{
    m_timer.stop();
}