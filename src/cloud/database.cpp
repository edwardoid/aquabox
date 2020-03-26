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

#include "database.h"
#include "settings.h"
#include <mongocxx/uri.hpp>
#include <mongocxx/logger.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/exception/exception.hpp>
#include <bsoncxx/stdx/make_unique.hpp>
#include <bsoncxx/builder/stream/document.hpp>

#include "log.h"

using namespace aquabox;


class LoggerAdapter: public mongocxx::logger
{
   public:
    LoggerAdapter() {}

    void operator()(mongocxx::log_level level,
                    bsoncxx::stdx::string_view domain,
                    bsoncxx::stdx::string_view message) noexcept override {
        if (level >= mongocxx::log_level::k_trace)
            return;
        LOG_DEBUG << '[' << mongocxx::to_string(level) << '@' << domain << "] " << message << '\n';
    }
};


Database::Database()
{
    mongocxx::instance(bsoncxx::stdx::make_unique<LoggerAdapter>());
    if(!connect()) {
        LOG_ERROR << "Can't connect to " << m_client.uri().to_string();
    } else {
        LOG_INFO << "Connected to " << m_client.uri().to_string();
    }
}

Database& Database::i()
{
    static Database instance;

    return instance;
}

bool Database::connect()
{
    try {
        const auto uri = mongocxx::uri(Settings::string(Settings::Keys::DATABASE_URI, Settings::Defaults::DATABASE_URI));

        mongocxx::options::client client_options;
        if (uri.ssl()) {
            mongocxx::options::ssl ssl_options;
            ssl_options.allow_invalid_certificates(true);
            client_options.ssl_opts(ssl_options);
        }

        m_client = std::move(mongocxx::client(uri, client_options));
        if (!m_client) {
            return false;
        }
        m_aquabox = m_client[Settings::string(Settings::Keys::DATABASE_NAME, Settings::Defaults::DATABASE_NAME)];
        return (bool)m_client;
    }
    catch(mongocxx::exception& e) {
        LOG_ERROR << e.what();
    }
    return false;
}

bool Database::wasProduced(std::string id) {
    LOG_DEBUG << "Trying to determine was box " << id << " produced or not...";
    bool res = (bool)(m_aquabox[Settings::string(Settings::Keys::HW_COLLECTION_NAME, Settings::Defaults::HW_COLLECTION_NAME)].find_one(bsoncxx::builder::stream::document{} << "serial" << id << bsoncxx::builder::stream::finalize));
    if (!res) {
        LOG_DEBUG << "Box " << id << " was not produced!";
    }
    return res;
}

void Database::setConnectedApps(std::string id, std::vector<std::string> appIds)
{
    auto applications = bsoncxx::builder::basic::array{};

    for (const auto& app : appIds) {
        applications.append(app);
    }
    m_aquabox[Settings::string(Settings::Keys::HW_COLLECTION_NAME, Settings::Defaults::HW_COLLECTION_NAME)]
    .update_one(bsoncxx::builder::stream::document{} << "serial" << id << bsoncxx::builder::stream::finalize,
                bsoncxx::builder::stream::document{} << "$set" << bsoncxx::builder::stream::open_document <<
                        "activeApps" << applications << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize);
}

void Database::setStreamAvailable(std::string id, std::string stream, bool available)
{
    m_aquabox[Settings::string(Settings::Keys::HW_COLLECTION_NAME, Settings::Defaults::HW_COLLECTION_NAME)]
    .update_one(bsoncxx::builder::stream::document{} << "serial" << id << bsoncxx::builder::stream::finalize,
                bsoncxx::builder::stream::document{} << "$set" << bsoncxx::builder::stream::open_document <<
                        stream << available << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize);
}

nlohmann::json Database::firmwareVersions(std::string version)
{
    nlohmann::json versions({{ "current", version }});
    return versions;
}
