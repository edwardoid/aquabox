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

    using RuleId = std::string;
    struct RuleSchedulerData
    {
        RuleId id;
        ValueChange change;
    };
    class RuleScheduler : protected Scheduler<RuleSchedulerData>
    {
    public:
        RuleScheduler();
        void recover();
        void add(const Rule& rule);
        void remove(const Rule& rule);
        void trigger();
        void updateDeviceRules(const std::string& deviceId);
        inline void beginTransaction() { Scheduler<RuleSchedulerData>::beginTransaction();  }
        inline void endTransaction() { Scheduler<RuleSchedulerData>::endTransaction();  }
    private:
        virtual void begin() override;
        virtual void finish() override;

        virtual bool ready(const int& id, std::time_t wasReadyAt, const RuleSchedulerData& data) override;

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