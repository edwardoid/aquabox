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

#include "message_formatter.h"
#include <stdint.h>

namespace aquabox
{
    class GetDevicesFormatter: public MessageFormatter
    {
    public:
        GetDevicesFormatter(const Command& cmd, const TgBot::Api& api, std::int64_t chatId);
        virtual void operator() (const Command& cmd, bool ok) const override;
    };
}