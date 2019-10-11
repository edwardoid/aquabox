#include "cloud_stream.h"
#include "log.h"

using namespace aquabox;


CloudStream::CloudStream(std::string url)
    : SimpleWeb::SocketClient<SimpleWeb::WS>(url)
    , m_url(url)
    , m_attached(false)
{
    on_open = [this](std::shared_ptr<Connection> conenction) { onOpen(connection); };

    on_message = [this](std::shared_ptr<Connection> connection, std::shared_ptr<InMessage> message) { onMessage(connection, message); };

    on_close = [this](std::shared_ptr<CloudStream::Connection> connection, int status, const std::string & reason) { onClose(connection, status, reason); };

    on_error = [this](std::shared_ptr<CloudStream::Connection> connection, const SimpleWeb::error_code & code) { onError(connection, code); };
}

void CloudStream::attach()
{
    LOG_DEBUG << "Attaching to " << m_url;
    SimpleWeb::SocketClient<SimpleWeb::WS>::start();
}

bool CloudStream::isAttached() const
{
    return m_attached;
}

void CloudStream::detach()
{
    LOG_DEBUG << "Dettaching from " << m_url;
    SimpleWeb::SocketClient<SimpleWeb::WS>::stop();
}

CloudStream::~CloudStream()
{
    detach();
}

void CloudStream::onOpen(std::shared_ptr<CloudStream::Connection> connection)
{
    m_attached = true;
}

void CloudStream::onMessage(std::shared_ptr<CloudStream::Connection> connection, std::shared_ptr<CloudStream::InMessage> message)
{
    m_attached = true;    
}

void CloudStream::onClose(std::shared_ptr<CloudStream::Connection> connection, int status, const std::string &reason)
{
    LOG_INFO << "Closed connection to " << m_url << " because: " << reason << "(" << status << ")";
    if (m_attached) {
        detach();
    }
    m_attached = false;

}

void CloudStream::onError(std::shared_ptr<CloudStream::Connection>, const SimpleWeb::error_code &code)
{
    LOG_INFO << "Error occured in connection to " << m_url << " error code " << code.message();
    if (m_attached) {
        detach();
    }
    m_attached = false;
}
