/*
 * AquaBox
 * Copyright (C) 2019  Eduard Sargsyan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    , m_rulesAppender("./../log/rules.log", BYTES_IN_MB, 3)
    , m_rulesLogger(plog::Severity::debug)
{
    m_rulesLogger.addAppender(&m_rulesAppender);
}

void RulesTimer::start()
{
    m_timer.start([this](const Timer::ClockTimePoint& when){
        AquaBox::i().doLater([this, when]() {
            RulesMap rulesToDelete;
            for(auto rule : AquaBox::i().rules()) {
                const Time at = rule.second->getRepeat().getDeleteAfter();
                const RepeatUnit unit = rule.second->getRepeat().getUnit();
                if(at != 0 &&
                   at.getDetails().from(TimeDetails(Timer::Clock::to_time_t(when)),
                                        true, true, true, true) >=0) {
                    rulesToDelete.insert({ rule.first, rule.second });
                }
                else if((bool) (rule.second->getEnabled())) {
                    RuleExecutor re(rule.second, when);
                    re();
                }
            }

            for(auto rule : rulesToDelete) {
                AquaBox::i().removeRule(rule.second);
            }

            for(auto& dev : AquaBox::i().devices()) {
                auto relay = std::dynamic_pointer_cast<Relay>(dev.second);
                if (relay == nullptr) {
                    continue;
                }
                bool isOn = relay->turnedOn();
                if(relay->performPendingAction()) {
                    LOG_DEBUG_IF_(1, true) << "Device" << (std::string)(relay->getName()) << " changed to Turned " << (relay->turnedOn() ? "ON" : "OFF");
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