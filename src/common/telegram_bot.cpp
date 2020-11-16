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
#include "log.h"
#include <tgbot/tools/StringTools.h>
#include "settings.h"

using namespace aquabox;

TelegramBot::TelegramBot(const std::string &token)
    : m_bot(token.empty() ? Settings::string(Settings::Keys::TELEGRAM_BOT_API, Settings::Defaults::TELEGRAM_BOT_API)
                          : token)
{
}

void TelegramBot::start()
{
    m_loop = std::thread([this]() {
        LOG_INFO << "Starting Telegram bot " << m_bot.getToken();
        setup();
        try
        {
            TgBot::TgLongPoll longPoll(m_bot);
            while (true)
            {
                longPoll.start();
            }
        }
        catch (TgBot::TgException &e) {
            LOG_FATAL << "Can't start Telegram bot: " << e.what();
        }
        catch (std::exception &e) {
            LOG_FATAL << "Can't start Telegram bot: " << e.what();
        }
        catch (...) {
            LOG_FATAL << "Starting of Te;egram bot failed. Check logs...";
        }
    });
    pthread_setname_np(m_loop.native_handle(), "TelegramBot");
}

TelegramBot::~TelegramBot()
{
    m_loop.join();
}

void TelegramBot::setup()
{
    //LOG_DEBUG << "Setting up handles for telegram bot " << m_bot.getToken();
    m_bot.getEvents().onCommand("start", [this](TgBot::Message::Ptr message) {
        m_bot.getApi().sendMessage(message->chat->id, "Hi!");
    });
    m_bot.getEvents().onAnyMessage([this](TgBot::Message::Ptr message) {
        LOG_DEBUG << "Got message " << message->text;
        // if (StringTools::startsWith(message->text, "/start"))
        // {
        //     return;
        // }
        m_bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
    });
}