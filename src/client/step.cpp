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

#include "step.h"
#include <log.h>
#include "hardware/hardware.h"
#include "rule.h"

using namespace aquabox;

Step::Step()
{
}

nlohmann::json Step::value(std::string property) const
{
    if (property == STEP_PROPERTY) {
        return steps();
    }

    return Device::value(property);
};

void Step::setValue(std::string property, const nlohmann::json& value)
{
    if (property == STEP_PROPERTY)
    {
        if (value.is_number())
        {
            makeSteps(value);
        }
    }
}

int32_t Step::steps() const
{
    auto hw = Hardware::resolve(m_address);
    if (hw == nullptr) {
        LOG_ERROR << "Can't find device " << m_address;
        return 0;
    }

    auto impl = hw->implementation<IntegerDevice>();

    if (impl == nullptr) {
        LOG_ERROR << m_address << " does not implement IntegerDevice interface.";
        return 0;
    }

    return impl->get();
}

void Step::makeSteps(int32_t steps)
{
    auto hw = Hardware::resolve(m_address);
    if (hw == nullptr) {
        LOG_ERROR << "Can't find device " << m_address;
        return;
    }

    auto impl = hw->implementation<IntegerDevice>();

    if (impl == nullptr) {
        LOG_ERROR << m_address << " does not implement IntegerDevice interface.";
        return;
    }

    impl->set(steps);

    LOG_DEBUG << "Steps count updated to " << steps;
}


void Step::addPendingAction(uint64_t priority, ValueChange action)
{
    m_pendingActions.insert({priority, action});
}

bool Step::performPendingAction(ValueChange& action)
{
    if (m_pendingActions.empty()) {
        return false;
    }

    auto lb = m_pendingActions.lower_bound(0);

    ValueChange actionToPerform;
    actionToPerform.property = ON_OFF_PROPERTY;
    actionToPerform.value = 1;
    if (lb == m_pendingActions.begin()) {
        actionToPerform = m_pendingActions.rbegin()->second;
    } else {
        if (lb->first != 0) {
            --lb;
        }
        actionToPerform = lb->second;
    }

    m_pendingActions.clear();

    if (actionToPerform.value != 0 && steps() > 0) {
        return false;
    }

    makeSteps(actionToPerform.value);

    action = actionToPerform;
    
    return true;
}