#include "cloud_stream.h"

using namespace aquabox;


CloudStream::CloudStream(std::string name)
    : SimpleWeb::SocketClient<SimpleWeb::WS>("edwardoid.redirectme.net:5555/" + name)
{

}

void CloudStream::connect()
{
    SimpleWeb::SocketClient<SimpleWeb::WS>::start();
}

void CloudStream::disconnect()
{
    SimpleWeb::SocketClient<SimpleWeb::WS>::stop();
}

CloudStream::~CloudStream()
{
    disconnect();
}