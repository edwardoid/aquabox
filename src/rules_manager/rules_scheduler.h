/*
 * AquaBox
 * Copyright (C) 2019-2022  Eduard Sargsyan
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

#pragma once

#include "cron_scheduler.h"
#include <set>
#include <types/rule.h>

namespace aquabox
{
    class RuleScheduler : protected Scheduler<std::pair<std::string, ValueChange>>
    {
    public:
        RuleScheduler();
        void recover();
        void add(const Rule& rule);
        void remove(const Rule& rule);
        void trigger();
        void updateDeviceRules(const std::string& deviceId);
        inline void beginTransaction() { Scheduler<std::pair<std::string, ValueChange>>::beginTransaction();  }
        inline void endTransaction() { Scheduler<std::pair<std::string, ValueChange>>::endTransaction();  }
    private:
        virtual void begin() override;
        virtual void finish() override;

        virtual bool ready(const int& id, const std::pair<std::string, ValueChange>& data) override;

        void updateDeviceRule(const int deviceId);

    private:
        enum State
        {
            Recovering,
            Working
        } m_state
            = State::Recovering;

        std::unordered_map<std::string, ValueChange> m_changes;
        std::set<std::string> m_rulesToDelete;
    };
} // namespace aquabox