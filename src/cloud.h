#ifndef CLOUD_H
#define CLOUD_H

#include <client_http.hpp>
#include <unordered_map>
#include <memory>
#include "cloud_stream.h"

using WsClient = SimpleWeb::SocketClient<SimpleWeb::WS>;
using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;

namespace aquabox
{
    class Cloud
    {
    public:
        void connect();
        void disconnect();
        bool isRegistered();
        void doRegister();
        void heartheat();
    private:
        bool isConnected();
    private:
        std::unordered_map<std::string, std::shared_ptr<CloudStream>> m_streams;
    };
}

#endif // CLOUD_H