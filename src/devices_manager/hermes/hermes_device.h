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

#include "hermes_hardware.h"

namespace aquabox
{
    template<class Base>
    class HermesDevice : public Base
    {
    public:
        HermesDevice() = default;
        void setupProperties(std::vector<std::string> props)
        {
            std::set<std::string> all;
            for (auto p : Base::properties())
                all.insert(p);

            for (auto& p : props) {
                if (p.empty())
                    continue;
                if (p[0] == '_')
                    continue;
                if (p == "type")
                    continue;
                if (p == "class")
                    continue;
                all.insert(p);
            }
            for (auto& p : all) {
                m_props.push_back(p);
            }
        }
        virtual std::vector<std::string> properties() const override
        {
            if (m_props.empty())
                return Base::properties();
            return m_props;
        }
        virtual nlohmann::json value(const std::string& property) const override
        {
            if (property.size() > 0 && property[0] == '+') {
                std::shared_ptr<HermesHardware> hw = std::dynamic_pointer_cast<HermesHardware>(Hardware::resolve(Base::address()));
                if (hw != nullptr) {
                    return hw->callInProgress(property) ? 1 : 0;
                }
                return 0;
            }
            return Base::value(property);
        }
        virtual bool setValue(const std::string& property, const nlohmann::json& value) override
        {
            if (property.empty()) {
                return false;
            }

            if (property[0] == '+') {
                std::shared_ptr<HermesHardware> hw = std::dynamic_pointer_cast<HermesHardware>(Hardware::resolve(Base::address()));
                if (hw != nullptr) {
                    return hw->call(property);
                }
                return true;
            }
            return Base::setValue(property, value);
        }

    private:
        std::vector<std::string> m_props;
    };
} // namespace aquabox