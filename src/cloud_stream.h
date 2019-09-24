#ifndef CLOUD_STREAM_H
#define CLOUD_STREAM_H

#include <client_ws.hpp>

namespace aquabox
{
    class CloudStream: SimpleWeb::SocketClient<SimpleWeb::WS>
    {
    public:
        CloudStream(std::string name);
        void connect();
        void disconnect();
        ~CloudStream();
    private:
    };
}

#endif // CLOUD_STREAM