#ifndef BOXES_MANAGER_H
#define BOXES_MANAGER_H

#include <string>
#include <unordered_map>
#include <memory>
#include <server_http.hpp>
#include <server_ws.hpp>
#include "route.h"

namespace aquabox
{
    class BoxesManager
    {
        using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
        using WSServer = SimpleWeb::SocketServer<SimpleWeb::WS>;

        BoxesManager() {};
        BoxesManager(const BoxesManager&) = delete;
        BoxesManager(BoxesManager&&) = delete;
        BoxesManager& operator = (const BoxesManager&) = delete;
        BoxesManager& operator = (BoxesManager&&) = delete;
    public:
        static bool start();
        static bool isRegistered(std::string id);
        static bool isValidId(std::string id);
        static bool isConnected(std::string id);
        static std::shared_ptr<class BoxConnection> get(std::string id);
    private:
        bool acceptConnectionFromHW(std::string id, std::shared_ptr<WSServer::Connection> connection);
        void setupRoutes();
    private:
        static BoxesManager self;
        HttpServer m_http;
        WSServer m_ws;
        Route m_register;
        Route m_login;
        Route m_log;
        Route m_events;
        Route m_commands;
        std::unordered_map<std::string, std::shared_ptr<class BoxConnection>> m_connections;
    };
}


#endif // BOXES_MANAGER_H
