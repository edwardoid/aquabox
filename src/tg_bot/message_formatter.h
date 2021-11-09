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

#include <common/command.h>
#include <tgbot/tgbot.h>
#include <sstream>
#include <unordered_map>

namespace aquabox
{
    class MessageFormatter
    {
    public:
        MessageFormatter(const Command& cmd, const TgBot::Api& api, std::int64_t chatId): m_cmd(cmd), m_api(api), m_chatId(chatId) {}
        virtual void operator() (const Command& cmd, bool ok) const = 0;
    protected:
        std::string bold(const std::string txt) const
        {
            return std::string("<b>") + txt + "</b>";
        }
        std::string italic(const std::string txt) const
        {
            return std::string("<i>") + txt + "</i>";
        }

        const std::string emoji(const std::string txt) const
        {
            static const std::unordered_map<std::string, std::string> emojis({
                { "true", "✅" },
                { "false", "⛔" },
                { "yes", "✅" },
                { "no", "⛔" },
                { "on", "✅" },
                { "off", "⛔" },
                { "sad", "😞" },
                { "filter", "💩" },
                { "poo", "💩" },
                { "light", "💡"},
                { "co2", "🌱"},
                { "plant", "🌱"},
                { "food", "🍔" },
                { "feeder", "🍔" },
                { "food", "🍔" },
                { "sensor", "📊" },
                { "ph", "📊" },
                { "ph", "📊" },
                { "thermometer", "🌡️" },
                { "us", "🇺🇸" },
                { "ru", "🇷🇺" },
                { "am", "🇦🇲" },
                { "warning", "⚠️"},
                { "info", "ℹ️"},
                { "error", "🔴" },
                { "cool", "😎" }
            });

            return emojis.count(txt) ? emojis.at(txt) : txt;
        }
    protected:
        const Command& m_cmd;
        const TgBot::Api& m_api;
        std::int64_t m_chatId;
    };
}