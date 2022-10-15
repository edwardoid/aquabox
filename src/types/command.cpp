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

#include "command.h"


namespace aquabox
{
    void to_json(nlohmann::json& j, const BaseCommand& command)
    {
        try {
            j.emplace("data", command.data());
            j.emplace("params", command.parameters());
            j.emplace("command", command.operation());
        } catch (const nlohmann::json::exception& ex) {

        }
    }

    void from_json(const nlohmann::json& j, BaseCommand& command)
    {
        try {
            if (j.count("data")) {
                command.setData(j["data"]);
            }

            command.setOperation((Operation)j["command"]);
            for (const auto entry : j["params"].items()) {
                std::string key = (std::string)(entry.key());
                std::string value = (std::string)(entry.value());
                command.setParameter(key, value);
            }
        } catch (const nlohmann::json::exception& ex) {

        }
    }

    void to_json(nlohmann::json& j, const CommandRequest& cr)
    {
        to_json(j, (const BaseCommand&) cr);
        to_json(j, (const BaseStreamRequest&) cr);
    }

    void from_json(const nlohmann::json& j, CommandRequest& cr)
    {
        from_json(j, (BaseCommand&) cr);
        from_json(j, (BaseStreamRequest&) cr);
    }

    void to_json(nlohmann::json& j, const CommandResult& cr)
    {
        to_json(j, (const BaseCommand&) cr);
        to_json(j, (const BaseStreamResponse&) cr);
    }

    void from_json(const nlohmann::json& j, CommandResult& cr)
    {
        from_json(j, (BaseCommand&) cr);
        from_json(j, (BaseStreamResponse&) cr);
    }
}

using namespace aquabox;

void BaseCommand::setParameters(const std::unordered_map<std::string, std::string>& params)
{
    for (auto p : params) {
        m_params.insert({p.first, p.second});
    }
}

const std::unordered_map<std::string, std::string>& BaseCommand::parameters() const
{
    return m_params;
}

void BaseCommand::setParameter(const std::string& name, const std::string& value)
{
    m_params.insert({name, value});
}

bool BaseCommand::hasParameter(const std::string& name) const
{
    auto pos = m_params.find(name);
    return pos != m_params.end() && !pos->second.empty();
}

const std::string& BaseCommand::getParameter(const std::string& name) const
{
    auto p = m_params.find(name);
    if (p == m_params.end()) {
        static const std::string empty;
        return empty;
    }
    return p->second;
}

void BaseCommand::setOperation(Operation op)
{
    m_operation = op;
}

Operation BaseCommand::operation() const
{
    return m_operation;
}
