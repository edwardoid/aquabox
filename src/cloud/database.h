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

#ifndef DATABASE_H
#define DATABASE_H

#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <string>
#include <nlohmann/json.hpp>

namespace aquabox
{
    class Database
    {
    public:
        static Database& i();
        bool wasProduced(const std::string id);
        bool isApplicationAuthorized(std::string app, std::string box) { return true; }
        void setConnectedApps(std::string id, std::vector<std::string> appId);
        void setStreamAvailable(std::string id, std::string stream, bool available);
        nlohmann::json firmwareVersions(std::string version);
    protected:
        Database();
        Database(const Database&) = delete;
        Database(Database&&) = delete;
        Database& operator = (const Database) = delete;
        Database& operator = (Database&&) = delete;

        bool connect();
    private:
        mongocxx::client m_client;
        mongocxx::database m_aquabox;
    };
}

#endif // DATABASE_H