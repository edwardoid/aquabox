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

#include "telegram_bot.h"
#include "get_devices_formatter.h"
#include <common/log.h>
#include <micro_clients/settings.h>
#include <thread>

using namespace aquabox;

TelegramBot::TelegramBot() {}

void TelegramBot::start()
{
    std::string api_key = Singleton<ISettings>::i()->get("TG_API_KEY", std::string("1406003356:AAGsrEfW77j7oUbApyev6ulmjWlPuHahrv8"));
    m_bot = std::make_shared<TgBot::Bot>(api_key);
    m_bot->getEvents().onCommand("devices", [this](TgBot::Message::Ptr message) {
        auto client = m_server.get("testDev");
        CommandRequest devices;
        devices.rqid = 0;
        devices.setOperation(Operation::GetDevices);
        client->runCommand(devices, GetDevicesFormatter(m_bot->getApi(), message->chat->id));
    });

    m_bot->getEvents().onCommand("list_rules", [this](TgBot::Message::Ptr message) {
        std::string reply = "You have ";
        reply += std::to_string(0);
        reply += " rules!";
        m_bot->getApi().sendMessage(message->chat->id, reply);
    });

    m_bot->getEvents().onCommand("subscribe", [this](TgBot::Message::Ptr message) {
        m_chats.insert(message->chat->id);
        m_bot->getApi().sendMessage(message->chat->id, "Subscribed!");
    });

    while (1) {
        try {
            LOG_INFO << m_bot->getApi().getMe()->username;
            m_bot->getApi().deleteWebhook();

            TgBot::TgLongPoll longPoll(*(m_bot.get()));
            while (true) {
                LOG_DEBUG << "Long poll started";
                longPoll.start();
            }
        }
        catch (std::exception& e) {
            LOG_ERROR << e.what();
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
