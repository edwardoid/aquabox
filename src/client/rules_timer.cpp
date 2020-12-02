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
#include "options.h"
#include <set>
#include <plog/Log.h>
#include <plog/Severity.h>
using namespace aquabox;

RulesTimer::RulesTimer()
    : m_timer("RulesTimer")
    , m_rulesAppender("./../log/rules.log", BYTES_IN_MB, 3)
    , m_rulesLogger(plog::Severity::debug)
    , m_skip(false)
{
    if (!Options::Service)
        m_rulesLogger.addAppender(&m_rulesAppender);
    else
        m_rulesLogger.addAppender(&m_consoleAppender);
}

void RulesTimer::start()
{
    m_timer.start([this](const Timer::ClockTimePoint& when){
        bool shouldSkip;
        if (m_skip.compare_exchange_strong(shouldSkip, true)) {
            return;
        }
        Singleton<AquaBox>::get()->doLater("rule_executor", [this, when]() {
            processRules(when);
            applyChanges();
            m_skip.store(false);
        });
    }, std::chrono::seconds(1)); // once per second
}

void RulesTimer::processRules(const Timer::ClockTimePoint& when)
{
    std::vector<std::string> rulesToDelete;
    std::vector<std::string> rulesToExec;
    RulesMap& allRules = Singleton<AquaBox>::get()->rules();
    allRules.for_each([&rulesToDelete, &rulesToExec, &when](auto id, auto rule) {
        const Time at = rule->getRepeat().getDeleteAfter();
        const RepeatUnit unit = rule->getRepeat().getUnit();
        if(at != 0 &&
            at.getDetails().from(TimeDetails(Timer::Clock::to_time_t(when)),
                                true, true, true, true) >=0) {
            rulesToDelete.push_back(id);
        }
        else if((bool) (rule->getEnabled())) {
            rulesToExec.push_back(id);
        }
    });

    for(auto id : rulesToExec) {
        auto rule = Singleton<AquaBox>::get()->findRule(id);
        if (rule) {
            RuleExecutor re(rule, when);
            re();
        }
    }

    for(auto id : rulesToDelete) {
        Singleton<AquaBox>::get()->removeRule(id);
    }
}

void RulesTimer::applyChanges()
{
    auto box = Singleton<AquaBox>::get();
    auto devices = box->devices().keys();

    for(auto id : devices) {
        auto dev = box->findDevice(id);
        if (dev == nullptr) {
            continue;
        }
        auto relay = std::dynamic_pointer_cast<Relay>(dev);
        if (relay == nullptr) {
            continue;
        }
        bool isOn = relay->turnedOn();
        if(relay->performPendingAction()) {
            LOG_DEBUG_IF_(1, true) << "Device" << (std::string)(relay->getName()) << " changed to Turned " << (relay->turnedOn() ? "ON" : "OFF");
        }
        if (isOn != relay->turnedOn()) {
            box->postUpdate(std::make_shared<RuleTriggeredEvent>(relay));
        }
    }
}

void RulesTimer::stop()
{
    m_timer.stop();
}