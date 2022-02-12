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

#include "cloud_service.h"
#include "applications_api.h"
#include "database.h"
#include "hardware_api.h"
#include "telegram_bot.h"
#include <common/singleton.h>
#include <micro_clients/settings.h>

#define LOG_FILE_SIZE_IN_MB 10
#define BYTES_IN_MB 1000000
#define MAX_LOG_FILES 50

using namespace aquabox;

CloudService CloudService::m_instance;

CloudService::CloudService()
    : m_fileAppender("./aquabox.log", LOG_FILE_SIZE_IN_MB * BYTES_IN_MB, MAX_LOG_FILES)
    , m_logAppender("./aquabox.csv", LOG_FILE_SIZE_IN_MB * BYTES_IN_MB, 1)
{}

CloudService& CloudService::i()
{
    return m_instance;
}

bool CloudService::start()
{
    plog::init(plog::Severity::debug, &m_consoleAppender);
    plog::init(plog::Severity::debug, &m_logAppender);

    LOG_INFO << "Starting Aquabox Cloud Service...";

    Singleton<TelegramBot>::available = !Settings::string(Settings::Keys::TELEGRAM_BOT_API, "").empty();
    auto telegram = Singleton<TelegramBot>::i();
    if (telegram != nullptr) {
        telegram->start();
    }

    Database::i();

    ApplicationsApi::start();
    return HardwareApi::start();
}
