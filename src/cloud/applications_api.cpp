#include "applications_api.h"
#include "box.h"
#include "application_connection.h"
#include "route.h"
#include "boxes_manager.h"
#include "database.h"
#include "settings.h"

using namespace aquabox;

ApplicationsApi ApplicationsApi::m_instance;

void ApplicationsApi::start()
{
    m_instance.m_base = Route().word("api").word("v1");

    static std::thread httpRoutine([]() {
        m_instance.startWebServer();
    });

    static std::thread wsRoutine([]() {
        m_instance.startStreamServer();
    });
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

    auto updates = m_base.sub().word("updates").string(":boxId").string(":appId");

    m_ws->endpoint[updates.url()].on_open = [this, updates](std::shared_ptr<WSServer::Connection> connection) {
        auto params = updates.extract(connection->path);
        auto boxId = params[":boxId"];
        auto appId = params[":appId"];
        LOG_INFO << "Connection opened from " << connection->remote_endpoint_address()
                 << " to box " << boxId << " by application " << params["appId"];
        if (!Database::i().isApplicationAuthorized(appId, boxId)) {
            connection->send_close(STREAM_CONNECTION_REJECTED);
            return;
        }
        LOG_INFO << "Application " << appId << " is authorized for box " << boxId;
        auto box = BoxesManager::get(boxId, true);
        box->addConnection(std::make_shared<ApplicationConnection>(*box, connection));
    };

    m_ws->endpoint[updates.url()].on_close = [this, updates](std::shared_ptr<WSServer::Connection> connection,
                                                             int status, const std::string & reason) {
        LOG_INFO << "Closed connection from " << connection->remote_endpoint_address();
        auto params = updates.extract(connection->path);
        auto boxId = params[":boxId"];
        removeConnection(boxId, connection);
    };

    m_ws->endpoint[updates.url()].on_error = [this, updates](std::shared_ptr<WSServer::Connection> connection,
                                                             SimpleWeb::error_code) {
        LOG_INFO << "Connection from " << connection->remote_endpoint_address() << " to " << connection->path << " closed due error.";
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

    for(auto c : box->connections()) {
        if ((*c) == connection) {
            box->removeConnection(c);
            break;
        }
    }
}
