#ifndef CLOUD_STREAM_H
#define CLOUD_STREAM_H

#include <client_ws.hpp>

namespace aquabox
{
    class CloudStream: SimpleWeb::SocketClient<SimpleWeb::WS>
    {
    public:
        CloudStream(std::string url);
        void attach();
        bool isAttached() const;
        void detach();
        ~CloudStream();
    protected:
        virtual void onOpen(std::shared_ptr<class Connection> connection);
        virtual void onMessage(std::shared_ptr<class Connection> connection, std::shared_ptr<class InMessage> message);
        virtual void onClose(std::shared_ptr<class Connection> connection, int, const std::string &);
        virtual void onError(std::shared_ptr<class Connection>, const SimpleWeb::error_code &);
    private:
        std::string m_url;
        bool m_attached;
    };
}

#endif // CLOUD_STREAM