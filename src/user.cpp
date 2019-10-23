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

#include "user.h"
#include "log.h"
#include "crypto.h"

using namespace aquabox;

bool User::init(const nlohmann::json& data)
{
    auto pos = data.find("user");
    if (pos == data.end()) {
        LOG_ERROR << "Can't find user name in JSON";
        return false;
    }
    if (!pos->is_string()) {
        LOG_ERROR << "User name is not string";
        return false;
    }

    m_user = pos->get<std::string>();

    pos = data.find("password");
    if (pos == data.end()) {
        LOG_ERROR << "Can't find password in JSON";
        return false;
    }
    if (!pos->is_string()) {
        LOG_ERROR << "Password is not string";
        return false;
    }

    m_password = pos->get<std::string>();
    return true;
}

const std::string& User::user() const
{
    return m_user;
}
        
const std::string& User::password() const
{
    return m_password;
}
        
std::string User::token() const
{
    const std::string raw = m_user + ":" + m_password;
    return SHA512::hash(raw);
}