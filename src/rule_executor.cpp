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

#include "rule_executor.h"
#include "rule.h"
#include "timer.h"
#include "log.h"
#include "enumerations.h"
#include "device.h"
#include "relay.h"
#include "time_details.h"
#include <ctime>

using namespace aquabox;

RuleExecutor::RuleExecutor(std::shared_ptr<Rule>& rule, Timer::ClockTimePoint now)
    : m_now(now), m_rule(rule)
{}

void RuleExecutor::operator() ()
{
    LOG_DEBUG << "Processing rule \"" << m_rule->getId() << "\" on " << m_rule->getDevice();
    const TimeDetails now(Timer::Clock::to_time_t(m_now));

    const RepeatUnit unit = m_rule->getRepeat().getUnit();
    auto dev = std::dynamic_pointer_cast<Relay>(m_rule->getDevice().resolve());

    if (dev == nullptr) {
        return;
    }

    if (!dev->getRulesEnabled().Value) {
        return;
    }
    
    for(const Action& act : *(m_rule->getActions()))
    {
        const TimeDetails at = act.getAt().getDetails();

        int64_t priority = at.from(now, true, unit >= RepeatUnit::Hour, unit >= RepeatUnit::Day, unit >= RepeatUnit::Month);
        
        if (priority >= 0) {
            LOG_DEBUG << "Triggering action " << Serializable<ActionType>::convert(act.getType()) << " for " << at.to_string();
            dev->addPendingAction(priority, act.getType());
        }
    }
}