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

#include "value_change.h"
#include <log.h>

namespace aquabox
{
    void to_json(nlohmann::json &j, const ValueChange &v)
    {
        j["property"] = v.property;
        j["value"] = v.value;
        j["when"] = v.when;
    }

    void from_json(const nlohmann::json &j, ValueChange &v)
    {
        try
        {
            j.at("property").get_to(v.property);
            j.at("value").get_to(v.value);
            v.when = j.at("when");
        }
        catch (const nlohmann::detail::exception &e)
        {
            LOG_ERROR << "Can't serialize Rule: " << e.what();
        }
    }
} // namespace aquabox