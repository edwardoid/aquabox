#ifndef STREAM_SERVER_H
#define STREAM_SERVER_H

#include <thread>
#include <websocketpp/server.hpp>
#include <websocketpp/config/core.hpp>
#include <websocketpp/config/config_builder.hpp>

namespace aquabox
{
    struct StreamServerConfig : public core::transport_config {
        typedef core::concurrency_type concurrency_type;
        typedef core::elog_type elog_type;
        typedef core::alog_type alog_type;
        typedef core::request_type request_type;
        typedef core::response_type response_type;

        static bool const enable_multithreading = true;
    };

    class StreamServer
    {
    public:
        StreamServer();
        void start();
        void stop();
    private:
        std::thread m_async;
    };
}

#endif // STREAM_SERVER_H