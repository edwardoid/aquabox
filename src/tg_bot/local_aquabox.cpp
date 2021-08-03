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

#include "local_aquabox.h"
#include "../micro_clients/services.h"
#include "../common/log.h"

using namespace aquabox;

LocalAquaboxAPI::LocalAquaboxAPI()
    : EventsBus("LocalAPI")
{
}

std::vector<Device> LocalAquaboxAPI::devices()
{
    return Singleton<IDevicesManager>::i()->all();
}

std::vector<Rule> LocalAquaboxAPI::rules()
{
    return Singleton<IRulesManager>::i()->all();
}

void LocalAquaboxAPI::start()
{
    std::string api_key = Singleton<ISettings>::i()->get("TG_API_KEY", "1406003356:AAGsrEfW77j7oUbApyev6ulmjWlPuHahrv8");
    m_bot = std::make_shared<TgBot::Bot>(api_key);
    m_bot->getEvents().onCommand("list_devices", [this](TgBot::Message::Ptr message) {
        std::string reply = "You have ";
        LOG_DEBUG << "Requesting devices";
        reply += std::to_string(devices().size());
        LOG_DEBUG << "Done";
        reply += " devices!";
        m_bot->getApi().sendMessage(message->chat->id, reply);
    });

    m_bot->getEvents().onCommand("list_rules", [this](TgBot::Message::Ptr message) {
        std::string reply = "You have ";
        reply += std::to_string(rules().size());
        reply += " rules!";
        m_bot->getApi().sendMessage(message->chat->id, reply);
    });

    m_bot->getEvents().onCommand("subscribe", [this](TgBot::Message::Ptr message) {
        m_chats.insert(message->chat->id);
        m_bot->getApi().sendMessage(message->chat->id, "Subscribed!");
    });


    try {
        printf("Bot username: %s\n", m_bot->getApi().getMe()->username.c_str());
        m_bot->getApi().deleteWebhook();

        TgBot::TgLongPoll longPoll(*(m_bot.get()));
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (std::exception& e) {
        printf("error: %s\n", e.what());
    }
}
