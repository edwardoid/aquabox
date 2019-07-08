#include "rules_timer.h"
#include "rule.h"
#include "relay.h"
#include "rule_executor.h"
#include "aquabox.h"
#include "event.h"
#include "update_events.h"
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
                if(at != 0 &&
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
                auto relay = std::dynamic_pointer_cast<Relay>(dev.second);
                if (relay == nullptr) {
                    continue;
                }
                bool isOn = relay->turnedOn();
                if(relay->performPendingAction()) {
                    LOG_DEBUG_IF_(1, true) << "Device" << relay->getName().Value << " changed to Turned " << (relay->turnedOn() ? "ON" : "OFF");
                }
                if (isOn != relay->turnedOn()) {
                    AquaBox::i().postUpdate(std::make_shared<RuleTriggeredEvent>(relay));
                }
            }
        });
    }, std::chrono::milliseconds(1000)); // once per second
}

void RulesTimer::stop()
{
    m_timer.stop();
}