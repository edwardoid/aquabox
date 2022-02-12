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

#include "types.h"
#include "device_type.h"
#include <common/log.h>

namespace aquabox
{
    namespace services
    {
        void to_json(nlohmann::json& j, const DeviceData& d)
        {
            try {
                j = nlohmann::json{{"id", d.id()},
                                   {"name", d.name()},
                                   {"index", d.index()},
                                   {"address", d.address()},
                                   {"rules_enabled", d.rulesenabled()},
                                   {"type", d.type()},
                                   {"class", d.clazz()},
                                   {"accepted", d.accepted()},
                                   {"available", d.available()}};
                nlohmann::json meta;
                for (const auto& m : d.meta()) {
                    meta[m.first] = m.second;
                }
                j["meta"] = meta;
            }
            catch (const nlohmann::detail::exception& e) {
                LOG_ERROR << "Can't serialize Device: " << e.what();
            }
        }

        void from_json(const nlohmann::json& j, DeviceData& d)
        {
            try {
                d.set_id(j.at("id"));
                d.set_name(j.at("name"));
                d.set_index(j.at("index"));
                d.set_address(j.at("address"));
                d.set_rulesenabled(j.at("rules_enabled"));
                d.set_type(j.at("type"));
                d.set_clazz(j.at("class"));
                d.set_accepted(j.at("accepted"));
                for (auto& m : j["meta"].get<nlohmann::json::object_t>()) {
                    LOG_INFO << "Updating meta " << m.first << " to  " << ((std::string) m.second);
                    d.mutable_meta()->insert({m.first, (std::string) m.second});
                }
            }
            catch (std::exception& e) {
                LOG_ERROR << "Parsing failed: " << e.what();
            }
            catch (...) {
                LOG_ERROR << "Parsing failed";
            }
        }
    } // namespace services
} // namespace aquabox