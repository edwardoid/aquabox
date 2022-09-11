/*
 * AquaBox
 * Copyright (C) 2019-2022 Eduard Sargsyan
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
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/stdx/make_unique.hpp>
#include <micro_clients/settings.h>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/logger.hpp>
#include <mongocxx/uri.hpp>

#include "log.h"

using namespace aquabox;

class LoggerAdapter : public mongocxx::logger
{
public:
    LoggerAdapter() {}

    void operator()(mongocxx::log_level level, bsoncxx::stdx::string_view domain, bsoncxx::stdx::string_view message) noexcept override
    {
        if (level >= mongocxx::log_level::k_trace)
            return;
        LOG_DEBUG << '[' << mongocxx::to_string(level) << '@' << domain << "] " << message << '\n';
    }
};

Database::Database()
{
    mongocxx::instance(bsoncxx::stdx::make_unique<LoggerAdapter>());
    if (!connect()) {
        LOG_ERROR << "Can't connect to " << m_client.uri().to_string();
    }
    else {
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
        if (uri.tls()) {
            mongocxx::options::tls tls_options;
            tls_options.allow_invalid_certificates(true);
            client_options.tls_opts(tls_options);
        }

        m_client = std::move(mongocxx::client(uri, client_options));
        if (!m_client) {
            return false;
        }
        m_aquabox = m_client[Settings::string(Settings::Keys::DATABASE_NAME, Settings::Defaults::DATABASE_NAME)];
        auto dbs = m_client.list_databases();
        return (bool) m_client && (dbs.begin() != dbs.end());
    }
    catch (mongocxx::exception& e) {
        LOG_ERROR << e.what();
    }
    return false;
}

bool Database::wasProduced(const std::string& id)
{
    LOG_DEBUG << "Trying to determine was box " << id << " produced or not...";
    bool res = (bool) (m_aquabox[Settings::string(Settings::Keys::HW_COLLECTION_NAME, Settings::Defaults::HW_COLLECTION_NAME)].find_one(
        bsoncxx::builder::stream::document{} << "serial" << id << bsoncxx::builder::stream::finalize));
    if (!res) {
        LOG_DEBUG << "Box " << id << " was not produced!";
    }
    return res;
}

bool Database::isApplicationAuthorized(const std::string& app, const std::string& box)
{
    bool res = (bool) (m_aquabox[Settings::string(Settings::Keys::HW_COLLECTION_NAME, Settings::Defaults::HW_COLLECTION_NAME)].find_one(
        bsoncxx::builder::stream::document{} << "serial" << box << "apps" << bsoncxx::builder::stream::open_document << "$elemMatch"
                                             << bsoncxx::builder::stream::open_document << "$eq" << app
                                             << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::close_document
                                             << bsoncxx::builder::stream::finalize));
    if (!res) {
        LOG_DEBUG << "App " << app << " is not authoriozed for box " << box << "!";
    }
    return res;
}

void Database::setConnectedApps(const std::string& id, const std::vector<std::string>& appIds)
{
    auto applications = bsoncxx::builder::basic::array{};

    for (const auto& app : appIds) {
        applications.append(app);
    }
    m_aquabox[Settings::string(Settings::Keys::HW_COLLECTION_NAME, Settings::Defaults::HW_COLLECTION_NAME)]
        .update_one(bsoncxx::builder::stream::document{} << "serial" << id << bsoncxx::builder::stream::finalize,
                    bsoncxx::builder::stream::document{} << "$set" << bsoncxx::builder::stream::open_document << "activeApps"
                                                         << applications << bsoncxx::builder::stream::close_document
                                                         << bsoncxx::builder::stream::finalize);
}

void Database::setStreamAvailable(const std::string& id, const std::string& stream, bool available)
{
    m_aquabox[Settings::string(Settings::Keys::HW_COLLECTION_NAME, Settings::Defaults::HW_COLLECTION_NAME)]
        .update_one(bsoncxx::builder::stream::document{} << "serial" << id << bsoncxx::builder::stream::finalize,
                    bsoncxx::builder::stream::document{} << "$set" << bsoncxx::builder::stream::open_document << stream << available
                                                         << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize);
}

nlohmann::json Database::firmwareVersions(const std::string& version)
{
    nlohmann::json versions({{"current", version}});
    return versions;
}

void Database::addTelegramChat(const std::string& id, int64_t chatId)
{
    m_aquabox[Settings::string(Settings::Keys::HW_COLLECTION_NAME, Settings::Defaults::HW_COLLECTION_NAME)]
        .update_one(bsoncxx::builder::stream::document{} << "serial" << id << bsoncxx::builder::stream::finalize,
                    bsoncxx::builder::stream::document{} << "$addToSet" << bsoncxx::builder::stream::open_document << "tgChats"
                                                         << chatId << bsoncxx::builder::stream::close_document
                                                         << bsoncxx::builder::stream::finalize);
}

void Database::removeTelegramChat(const std::string& id, int64_t chatId)
{
    m_aquabox[Settings::string(Settings::Keys::HW_COLLECTION_NAME, Settings::Defaults::HW_COLLECTION_NAME)]
        .update_one(bsoncxx::builder::stream::document{} << "serial" << id << bsoncxx::builder::stream::finalize,
                    bsoncxx::builder::stream::document{} << "$pull" << bsoncxx::builder::stream::open_document << "tgChats"
                                                         << chatId << bsoncxx::builder::stream::close_document
                                                         << bsoncxx::builder::stream::finalize);
}

std::set<int64_t> Database::telegramChats(const std::string& id)
{
    std::set<int64_t> chats;
    bsoncxx::stdx::optional<bsoncxx::document::value> cursor
        = m_aquabox[Settings::string(Settings::Keys::HW_COLLECTION_NAME, Settings::Defaults::HW_COLLECTION_NAME)].find_one(
            bsoncxx::builder::stream::document{} << "serial" << id << bsoncxx::builder::stream::finalize);
    if (!cursor) {
        return chats;
    }

    bsoncxx::document::view view = cursor.value().view();
    auto tgChats = view.find("tgChats");
    if (tgChats == view.end()) {
        return chats;
    }

    if (tgChats->type() != bsoncxx::type::k_array) {
        return chats;
    }

    bsoncxx::array::view ids = tgChats->get_array();
    for (auto i = ids.begin(); i != ids.end(); ++i) {
        if (i->type() == bsoncxx::type::k_int64) {
            chats.insert(i->get_int64());
        }
    }

    return chats;
}
