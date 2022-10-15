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

#include <types/base_stream_message.h>
#include <types/operation.h>

namespace aquabox
{
    class BaseCommand
    {
    public:
        BaseCommand() = default;
        void setOperation(Operation op);
        Operation operation() const;
        inline void setData(const nlohmann::json& data) { m_data = data; }
        const nlohmann::json& data() const { return m_data; }
        void setParameters(const std::unordered_map<std::string, std::string>& params);
        const std::unordered_map<std::string, std::string>& parameters() const;
        void setParameter(const std::string& name, const std::string& value);
        bool hasParameter(const std::string& name) const;
        const std::string& getParameter(const std::string& name) const;
    private:
        Operation m_operation;
        std::unordered_map<std::string, std::string> m_params;
        nlohmann::json m_data = nlohmann::json::object();
    };

    struct CommandRequest: public BaseCommand, public BaseStreamRequest
    {
        CommandRequest(): BaseStreamRequest(ApiMessageType::Command) {}
    };

    struct CommandResult:  public BaseCommand, public BaseStreamResponse
    {
        CommandResult(): BaseStreamResponse(ApiMessageType::Command) {}
    };

    void to_json(nlohmann::json& j, const BaseCommand& bc);
    void from_json(const nlohmann::json& j, BaseCommand& bc);

    void to_json(nlohmann::json& j, const CommandRequest& cr);
    void from_json(const nlohmann::json& j, CommandRequest& cr);

    void to_json(nlohmann::json& j, const CommandResult& cr);
    void from_json(const nlohmann::json& j, CommandResult& cr);
} // namespace aquabox
