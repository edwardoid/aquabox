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

#include "server_connection.h"
#include <services/aquabox_api.h>
#include <tgbot/tgbot.h>

namespace aquabox
{
    class TelegramBot
    {
    public:
        TelegramBot();
        void start();

    private:
        std::shared_ptr<TgBot::Bot> m_bot;
        std::set<std::int64_t> m_chats;
        ServerConnection m_server;
    };
} // namespace aquabox
