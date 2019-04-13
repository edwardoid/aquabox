#include "wsapi.h"

using namespace aquabox::ws;

DEFINE_WS_API_IMPL(Ping)
{
    connection->send("msg");
}