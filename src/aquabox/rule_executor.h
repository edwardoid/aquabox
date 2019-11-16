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

#ifndef RULE_EXECUTOR_H
#define RULE_EXECUTOR_H

#include "timer.h"
#include "rule.h"

#include <memory>

namespace aquabox
{
    class RuleExecutor
    {
    public:
        RuleExecutor(std::shared_ptr<class Rule>& rule, Timer::ClockTimePoint now);
        RuleExecutor(const RuleExecutor&) = default;
        RuleExecutor(RuleExecutor&& src) = default;
        void operator() ();
    private:
        const Timer::ClockTimePoint m_now;
        std::shared_ptr<class Rule> m_rule;
    };
}

#endif // RULE_EXECUTOR_H