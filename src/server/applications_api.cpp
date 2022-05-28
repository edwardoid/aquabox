#include "applications_api.h"
#include "application_connection.h"
#include "box.h"
#include "boxes_manager.h"
#include "database.h"
#include "route.h"
#include "telegram_bot.h"
#include <micro_clients/settings.h>

using namespace aquabox;

void ApplicationsApi::start()
{
    m_base = Route().word("api").word("v1");

    m_httpRoutine = std::move(std::thread(std::bind(&ApplicationsApi::startWebServer, this)));

    m_streamRoutine = std::move(std::thread(std::bind(&ApplicationsApi::startStreamServer, this)));
}

void ApplicationsApi::startWebServer()
{
    m_http = std::make_shared<HttpServer>();
    m_http->config.port = Settings::integer(Settings::Keys::HTTP_PORT, Settings::Defaults::REST_PORT);
    LOG_INFO << "Starting web server on " << m_http->config.address << ":" << m_http->config.port;
    RESTApi.init(m_http, m_base);

    m_http->start();
}

void ApplicationsApi::startStreamServer()
{
    m_ws = std::make_shared<WSServer>();
    m_ws->config.port = Settings::integer(Settings::Keys::STREAM_PORT, Settings::Defaults::WS_PORT);
    LOG_INFO << "Starting websocket server" << m_ws->config.address << ":" << m_ws->config.port;

    auto updates = m_base.sub().string(":boxId").string(":appId").word("updates");

    m_ws->endpoint[updates.url()].on_open = [this, updates](std::shared_ptr<WSServer::Connection> connection) {
        auto params = updates.extract(connection->path);
        auto boxId = params[":boxId"];
        auto appId = params[":appId"];
        LOG_INFO << "Connection opened from " << connection->remote_endpoint().address().to_string() << " to box " << boxId
                 << " by application " << params["appId"];
        if (!Database::i().isApplicationAuthorized(appId, boxId)) {
            connection->send_close(STREAM_CONNECTION_REJECTED);
            return;
        }
        if (BoxesManager::get(boxId, false) == nullptr) {
            connection->send_close(BOX_IS_OFFLINE);
            return;
        }
        LOG_INFO << "Application " << appId << " is authorized for box " << boxId;
        auto box = BoxesManager::get(boxId, true);
        box->addConnection(std::make_shared<ApplicationConnection>(*box, connection));
    };

    m_ws->endpoint[updates.url()].on_close =
        [this, updates](std::shared_ptr<WSServer::Connection> connection, int status, const std::string& reason) {
            LOG_INFO << "Closed connection from " << connection->remote_endpoint().address().to_string();
            auto params = updates.extract(connection->path);
            auto boxId = params[":boxId"];
            removeConnection(boxId, connection);
        };

    m_ws->endpoint[updates.url()].on_error = [this, updates](std::shared_ptr<WSServer::Connection> connection, SimpleWeb::error_code) {
        LOG_INFO << "Connection from " << connection->remote_endpoint().address().to_string() << " to " << connection->path
                 << " closed due error.";
        auto params = updates.extract(connection->path);
        auto boxId = params[":boxId"];
        removeConnection(boxId, connection);
    };

    m_ws->endpoint[updates.url()].on_message = [this](std::shared_ptr<WSServer::Connection> connection,
                                                      std::shared_ptr<WSServer::InMessage> in_message) {
        LOG_DEBUG << "Got message from application. Ignoring...";
    };

    m_ws->start();
}

void ApplicationsApi::removeConnection(std::string boxId, std::shared_ptr<WSServer::Connection> connection)
{
    auto box = BoxesManager::get(boxId);
    if (box == nullptr) {
        LOG_ERROR << "Can't remove connection to box " << boxId << " because it is missed";
        return;
    }

    for (auto c : box->connections()) {
        if ((*c) == connection) {
            box->removeConnection(c);
            break;
        }
    }
}
