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

#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <nlohmann/json.hpp>
#include <set>
#include <string>
namespace aquabox
{
    class Database
    {
    public:
        static Database& i();
        bool wasProduced(const std::string& id);
        bool isApplicationAuthorized(const std::string& app, const std::string& box);
        void setConnectedApps(const std::string& id, const std::vector<std::string>& appId);
        void setStreamAvailable(const std::string& id, const std::string& stream, bool available);
        nlohmann::json firmwareVersions(const std::string& version);
        std::set<int64_t> telegramChats(const std::string& id);

    protected:
        Database();
        Database(const Database&) = delete;
        Database(Database&&) = delete;
        Database& operator=(const Database) = delete;
        Database& operator=(Database&&) = delete;

        bool connect();

    private:
        mongocxx::client m_client;
        mongocxx::database m_aquabox;
    };
} // namespace aquabox
