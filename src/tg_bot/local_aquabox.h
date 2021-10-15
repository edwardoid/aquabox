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

#include <services/aquabox_api.h>
#include <tgbot/tgbot.h>

namespace aquabox {
class LocalAquaboxAPI final : public IAquaboxApi {
public:
    LocalAquaboxAPI();
    void start();
    virtual std::vector<DeviceRef> devices() override;
    virtual std::vector<RuleRef> rules() override;

private:
    std::shared_ptr<TgBot::Bot> m_bot;
    std::set<std::int64_t> m_chats;
};
}
