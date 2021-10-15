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

#include "hermes_hardware.h"

using namespace aquabox;

HermesHardware::HermesHardware(std::shared_ptr<HermesMaster> impl)
    : m_impl(impl)
{
}

bool HermesHardware::isReady()
{
    return m_impl != nullptr && m_impl->isConnected();
}

void HermesHardware::setOn()
{
    if (m_impl == nullptr) {
        return;
    }

    std::lock_guard<HermesMaster> lock(*m_impl);
    m_impl->setProperty<bool>("on", true);
}

void HermesHardware::setOff()
{
    if (m_impl == nullptr) {
        return;
    }

    std::lock_guard<HermesMaster> lock(*m_impl);
    m_impl->setProperty<bool>("on", false);
}

bool HermesHardware::isOn()
{
    if (m_impl == nullptr) {
        return false;
    }

    std::lock_guard<HermesMaster> lock(*m_impl);

    bool val = false;
    if (m_impl->getProperty<bool>("on", val)) {
        return val;
    }
    return false;
}

bool HermesHardware::set(int32_t val)
{
    if (m_impl == nullptr) {
        return false;
    }

    std::lock_guard<HermesMaster> lock(*m_impl);
    return m_impl->setProperty<int32_t>("times", val);
}

int32_t HermesHardware::get()
{
    if (m_impl == nullptr) {
        return 0;
    }

    std::lock_guard<HermesMaster> lock(*m_impl);

    int32_t val = 0;
    if (m_impl->getProperty<int32_t>("times", val)) {
        return val;
    } else {
        LOG_WARNING << "Can't get property 'times'.";
    }
    return 0;
}
