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

#include "converters.h"
#include "../types/rule.h"
#include "../types/value_change.h"
#include <devices.pb.h>
#include <rules.pb.h>

using namespace aquabox;

template<>
ValueChange services::Converter<services::RuleAction, ValueChange>::convert(const services::RuleAction& data)
{
    ValueChange res;
    res.property = data.property();
    res.cron = data.cron();
    res.value = data.value();
    return res;
}

template<>
services::RuleAction services::Converter<ValueChange, services::RuleAction>::convert(const ValueChange& data)
{
    services::RuleAction res;
    res.set_property(data.property);
    res.set_cron(data.cron);
    res.set_value(data.value);
    return res;
}

template<>
Rule services::Converter<services::RuleData, Rule>::convert(const services::RuleData& data)
{
    Rule rule;
    rule.setId(data.id());
    rule.setName(data.name());
    rule.setIndex(data.index());
    rule.setDevice(data.device());
    rule.setEnabled(data.enabled());
    rule.setCreatedAt(data.createdat());
    rule.setDeleteAfter(data.deleteafter());

    std::vector<ValueChange> actions;
    for (auto& a : data.actions()) {
        actions.push_back(Converter<services::RuleAction, ValueChange>::convert(a));
    }

    rule.setActions(actions);
    return rule;
}

template<>
services::RuleData services::Converter<Rule, services::RuleData>::convert(const Rule& data)
{
    services::RuleData rule;
    rule.set_id(data.id());
    rule.set_name(data.name());
    rule.set_index(data.index());
    rule.set_device(data.device());
    rule.set_enabled(data.enabled());
    rule.set_createdat(data.createdAt());
    rule.set_deleteafter(data.deleteAfter());

    std::vector<services::RuleAction> actions;
    for (auto& vc : data.actions()) {
        auto a = rule.add_actions();
        a->CopyFrom(Converter<ValueChange, services::RuleAction>::convert(vc));
    }

    return rule;
}