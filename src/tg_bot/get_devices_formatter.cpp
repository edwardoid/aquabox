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

#include "get_devices_formatter.h"
#include <common/log.h>
#include <sstream>
using namespace aquabox;

GetDevicesFormatter::GetDevicesFormatter(const Command& cmd, const TgBot::Api& api, std::int64_t chatId)
    : MessageFormatter(cmd, api, chatId)
{}

void GetDevicesFormatter::operator()(const Command& cmd, bool ok) const
{
    std::stringstream ss;
    try {
        if (cmd.response().is_object()) {
            ss << bold(italic("Available Devices")) << "\n";
            for (const nlohmann::json& d : cmd.response().at("devices")) {
                if (d.count("name") && d.count("type")) {
                    ss << "    Name: " << d["name"].get<std::string>() << "\n";
                    ss << "    Type: " << d["type"].get<std::string>() << "\n";
                    ss << "    Available: " << emoji(d["available"].dump()) << "\n";
                    ss << "-------------------------------\n";
                }
            }
        }
        else {
            ss << "Box is " << bold("not available") << " yet " << emoji("sad") << ". Try again later";
        }

        m_api.sendMessage(m_chatId, ss.str(), false, 0, std::make_shared<TgBot::GenericReply>(), "HTML");
    }
    catch (std::exception& e) {
        m_api.sendMessage(m_chatId, e.what());
    }
}