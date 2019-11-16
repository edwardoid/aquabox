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

#ifndef RULE_H
#define RULE_H

#include <string>
#include "repeat_timeout.h"
#include "action.h"
#include "time.h"
#include "device_reference.h"
#include "enumerations.h"
#include "property.h"

namespace aquabox
{
    /*
        {
            "id" : "someId",
            "device" : "dev01",
            "actions" : [
                {
                    "action" : "turn_on",
                    "at" : "09:00"
                },
                {
                    "action" : "turn_off",
                    "at" : "18:55"
                }
            ],
            "createdAt" : <date>,
            "expiresAt" : <date>,
            "expireIn" : "permanent"
            "repeat" : {
                "every" : "minute|hour|day|workday|month",
                "count" : <number>
            }
        }
    */

    class Rule: public ClassProperty
    {
    public:
        Rule();

        Rule(const Rule& src);
        Rule(Rule&& src);
        const Rule& operator = (const Rule& src);

        bool parseFile(const std::string& file);
        bool saveToFile(const std::string& file);

        property(Id, StringProperty)
        property(Name, StringProperty)
        property(Index, UnsignedNumberProperty)
        property(Device, DeviceReference)
        property(Enabled, BooleanProperty)
        property(CreatedAt, Time)
        property(LastRunAt, Time)
        property(Repeat, RepeatTimeout)
        property(Actions, ClassArrayProperty<Action>)
    };
}

#endif // RULE_H