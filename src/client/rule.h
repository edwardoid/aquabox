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

#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "value_change.h"
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

    class Rule
    {
    public:
        Rule() = default;
        ~Rule() = default;

        bool parseFile(const std::string& file);
        bool saveToFile(const std::string& file);

        const std::string& id() const;
        void setId(const std::string& id);
 
        const std::string& name() const;
        void setName(const std::string& name);

        uint32_t index() const;
        void setIndex(const uint32_t index);

        const std::string& device() const;
        void setDevice(const std::string& device);
        std::shared_ptr<class Device> resolveDevice() const;

        bool enabled() const;
        void setEnabled(bool enabled);

        uint64_t createdAt() const;
        void setCreatedAt(uint64_t createdAt);

        uint64_t lastRun() const;
        void setLastRun(uint64_t lastRun);

        std::time_t deleteAfter() const;
        void setDeleteAfter(std::time_t deleteAfter);

        const std::vector<ValueChange>& actions() const;
        void setActions(const std::vector<ValueChange>& actions);

    private:
        std::string m_id;
        std::string m_name;
        uint32_t m_index;
        std::string m_device;
        bool m_enabled;
        bool m_available;
        uint64_t m_createdAt;
        uint64_t m_lastRun;
        std::time_t m_deleteAfter;
        std::vector<ValueChange> m_actions;
    };

    void to_json(nlohmann::json& j, const Rule& r);
    void from_json(const nlohmann::json& j, Rule& r);
}
