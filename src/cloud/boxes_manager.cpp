#include "boxes_manager.h"
#include "box_connection.h"
#include "database.h"
#include "log.h"
#include "settings.h"

#include <thread>
#include <nlohmann/json.hpp>

using namespace aquabox;

#define REJECTED 0XDEAD

BoxesManager BoxesManager::self;

bool BoxesManager::start()
{
    LOG_INFO << "Starting Aquabox devices connection manager...";

    self.setupRoutes();

    static std::thread managerThread([](){
       BoxesManager::self.m_ws.config.port = Settings::integer(Settings::Keys::CLOUD_WS_PORT, Settings::Defaults::CLOUD_WS_PORT);
       LOG_INFO << "Starting Websocket server on " << self.m_ws.config.port;
       self.m_ws.start();
    });
    self.m_http.config.port = Settings::integer(Settings::Keys::CLOUD_PORT, Settings::Defaults::CLOUD_PORT);
    LOG_INFO << "Starting HTTP server on " << self.m_http.config.port;
    self.m_http.start();
    return true;
}

void BoxesManager::setupRoutes()
{
    const Route base = Route().word("hw").word("api").word("v1");
    m_register = base.sub().word("register");
    m_login = base.sub().word("login");


    m_http.resource[m_register.url()]["POST"] = [this](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        try {
            auto registerData = nlohmann::json::parse(request->content.string());
            if (!registerData.contains("id")) {
                response->write(SimpleWeb::StatusCode::client_error_bad_request, nlohmann::json({ { "error", "Box id is missing"} }).dump(4));
                return;
            }
            auto id = registerData["id"].get<std::string>();
            if (Database::i().wasProduced(id)) {
                response->write(SimpleWeb::StatusCode::success_accepted, nlohmann::json({
                                                                                            { "streams", { "log", "events", "commands"}},
                                                                                            { "id", id }
                                                                                        }).dump(4),
                                {{ "Content-Type", "application/json" }});
            } else {
                response->write(SimpleWeb::StatusCode::client_error_method_not_allowed);
            }
        }
        catch (nlohmann::json::exception& err) {
            response->write(SimpleWeb::StatusCode::client_error_bad_request, nlohmann::json({ { "error", err.what() } }).dump(4));
        }
        catch (std::exception& err) {
            response->write(SimpleWeb::StatusCode::server_error_internal_server_error, nlohmann::json({ { "error", err.what() } }).dump(4));
        }
        catch (...) {
            response->write(SimpleWeb::StatusCode::server_error_internal_server_error);
        }
    };

    m_http.resource[m_login.url()]["POST"] = [this](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {

    };

    m_log = base.sub().word("stream").string(":id").word("log");
    m_events = base.sub().word("stream").string(":id").word("events");
    m_commands = base.sub().word("stream").string(":id").word("commands");


    m_ws.endpoint[m_log.url()].on_open = [this](std::shared_ptr<WSServer::Connection> connection) {
        const auto id = m_log.extract(connection->path)[":id"];
        if(acceptConnectionFromHW(id, connection)) {
            m_connections[id]->logs = connection; 
        }
    };

    m_ws.endpoint[m_log.url()].on_close = [this](std::shared_ptr<WSServer::Connection> connection,
                                                 int status, const std::string & reason) {
        if (status == REJECTED) {
            return;
        }
        LOG_INFO << "Connection from " << connection->remote_endpoint.address().to_string() << " to " << connection->path << " closed.";
        const auto id = m_log.extract(connection->path)[":id"];
        auto ref = m_connections.find(id);
        if (ref == m_connections.end()) {
            return;
        }
        ref->second->logs.reset();
        if (ref->second->empty()) {
            m_connections.erase(ref);
        }
    };

    m_ws.endpoint[m_log.url()].on_error = [this](std::shared_ptr<WSServer::Connection> connection,
                                                 SimpleWeb::error_code) {
        LOG_INFO << "Connection from " << connection->remote_endpoint.address().to_string() << " to " << connection->path << " closed due error.";
        const auto id = m_log.extract(connection->path)[":id"];
        auto ref = m_connections.find(id);
        if (ref == m_connections.end()) {
            return;
        }
        ref->second->logs.reset();
        if (ref->second->empty()) {
            m_connections.erase(ref);
        }
    };

    m_ws.endpoint[m_log.url()].on_message = [this](std::shared_ptr<WSServer::Connection> connection,
                                                   std::shared_ptr<WSServer::InMessage> in_message) {
        LOG_DEBUG << "On " << connection->path << " got:\n\t\t" << in_message->string();
    };

    m_ws.endpoint[m_events.url()].on_open = [this](std::shared_ptr<WSServer::Connection> connection) {
        const auto id = m_events.extract(connection->path)[":id"];
        if(acceptConnectionFromHW(id, connection)) {
            m_connections[id]->events = connection; 
        }
    };

    m_ws.endpoint[m_events.url()].on_close = [this](std::shared_ptr<WSServer::Connection> connection,
                                                    int status, const std::string & reason) {
        if (status == REJECTED) {
            return;
        }
        LOG_INFO << "Connection from " << connection->remote_endpoint.address().to_string() << " to " << connection->path << " closed.";
        const auto id = m_events.extract(connection->path)[":id"];
        auto ref = m_connections.find(id);
        if (ref == m_connections.end()) {
            return;
        }
        ref->second->events.reset();
        if (ref->second->empty()) {
            m_connections.erase(ref);
        }
    };

    m_ws.endpoint[m_events.url()].on_error = [this](std::shared_ptr<WSServer::Connection> connection,
                                                 SimpleWeb::error_code) {
        LOG_INFO << "Connection from " << connection->remote_endpoint.address().to_string() << " to " << connection->path << " closed due error.";
        const auto id = m_events.extract(connection->path)[":id"];
        auto ref = m_connections.find(id);
        if (ref == m_connections.end()) {
            return;
        }
        ref->second->events.reset();
        if (ref->second->empty()) {
            m_connections.erase(ref);
        }
    };

    m_ws.endpoint[m_events.url()].on_message = [this](std::shared_ptr<WSServer::Connection> connection,
                                                      std::shared_ptr<WSServer::InMessage> in_message) {
        LOG_DEBUG << "On " << connection->path << " got:\n\t\t" << in_message->string();
    };

    m_ws.endpoint[m_commands.url()].on_open = [this](std::shared_ptr<WSServer::Connection> connection) {
        const auto id = m_commands.extract(connection->path)[":id"];
        if(acceptConnectionFromHW(id, connection)) {
            m_connections[id]->commands = connection; 
        }
    };

    m_ws.endpoint[m_commands.url()].on_close = [this](std::shared_ptr<WSServer::Connection> connection,
                                                      int status, const std::string & reason) {
        if (status == REJECTED) {
            return;
        }
        LOG_INFO << "Connection from " << connection->remote_endpoint.address().to_string() << " to " << connection->path << " closed.";
        const auto id = m_commands.extract(connection->path)[":id"];
        auto ref = m_connections.find(id);
        if (ref == m_connections.end()) {
            return;
        }
        ref->second->commands.reset();
        if (ref->second->empty()) {
            m_connections.erase(ref);
        }
    };

    m_ws.endpoint[m_commands.url()].on_error = [this](std::shared_ptr<WSServer::Connection> connection,
                                                      SimpleWeb::error_code) {
        LOG_INFO << "Connection from " << connection->remote_endpoint.address().to_string() << " to " << connection->path << " closed due error.";
        const auto id = m_commands.extract(connection->path)[":id"];
        auto ref = m_connections.find(id);
        if (ref == m_connections.end()) {
            return;
        }
        ref->second->commands.reset();
        if (ref->second->empty()) {
            m_connections.erase(ref);
        }
    };

    m_ws.endpoint[m_commands.url()].on_message = [this](std::shared_ptr<WSServer::Connection> connection,
                                                        std::shared_ptr<WSServer::InMessage> in_message) {

        LOG_DEBUG << "On " << connection->path << " got some data";
        
        auto id = m_commands.extract(connection->path)[":id"];
        auto ref = m_connections.find(id);
        if (ref == m_connections.end()) {
            return;
        }

        ref->second->ready(in_message->string());
    };
}

bool BoxesManager::isRegistered(std::string id)
{
}

bool BoxesManager::isValidId(std::string id)
{
}

bool BoxesManager::isConnected(std::string id)
{
}

bool BoxesManager::acceptConnectionFromHW(std::string id, std::shared_ptr<WSServer::Connection> connection)
{
    LOG_INFO << "Got connection from " << connection->remote_endpoint.address().to_string()
             << " to stream 'log' for " << id << " to " << connection->path;
    if (!Database::i().wasProduced(id)) {
        LOG_WARNING << "Rejecting connection from " << connection->remote_endpoint.address().to_string() << " to stream for " << id;
        connection->send_close(REJECTED);
        return false;
    }
    if (m_connections.find(id) == m_connections.end()) {
        m_connections.insert({id, std::make_shared<BoxConnection>(m_results)});
    }
    return true;
}

std::shared_ptr<BoxConnection> BoxesManager::get(std::string id)
{
    auto p = self.m_connections.find(id);
    return p == self.m_connections.end() ? nullptr : p->second;
}
