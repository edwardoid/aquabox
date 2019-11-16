#include "database.h"
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
        const auto uri = mongocxx::uri("mongodb://aquabox:azazello@edwardoid.redirectme.net");

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
        m_aquabox = m_client["aquabox"];
        return (bool)m_client;
    }
    catch(mongocxx::exception& e) {
        LOG_ERROR << e.what();
    }
    return false;
}

bool Database::wasProduced(std::string id) {
    return (bool)(m_aquabox["hw"].find_one(bsoncxx::builder::stream::document{} << "serial" << id << bsoncxx::builder::stream::finalize));
}