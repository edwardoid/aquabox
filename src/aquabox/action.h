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

#ifndef ACTION_H
#define ACTION_H

#include "action_type.h"
#include "property.h"
#include "time.h"

namespace aquabox
{
    class Action: public ClassProperty
    {
    public:
        Action(std::string name = "");
        Action(const Action& src);
        Action(Action&& src);
        const Action& operator = (const Action& src);

        property(At, Time);
        property(Type, EnumProperty<ActionType>);
    };
}

#endif // ACTION_H