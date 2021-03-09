/*
 * AquaBox
 * Copyright (C) 2019  Eduard Sargsyan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "socket_bus.h"
#include "../aquabox.h"
#include "hermes/hermes_incubator.h"
#include "hermes/hermes_hardware.h"
#include <unix_socket_proto_io.h>
#include <settings.h>
#include <log.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

using namespace aquabox;

SocketBus::SocketBus(const std::string& address)
    : HermesBus(address)
{}

SocketBus::~SocketBus()
{}

void SocketBus::loop()
{
    signal(SIGPIPE, SIG_IGN);
    int parentfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (parentfd < 0) {
        LOG_ERROR << "Can't open socket";
        return;
    }

    int optval = 1;
    ::setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR,
               (const void *)&optval, sizeof(int));

    struct sockaddr_in serveraddr;
    memset((char *)&serveraddr, 0, sizeof(serveraddr));

    int port = Settings::integer(Settings::Keys::SCT_BUS_PORT, Settings::Defaults::SCT_BUS_PORT);

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)port);
    if (::bind(parentfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
    {
        LOG_ERROR << "Can't bind to port " << port;
        return;
    }

    if (listen(parentfd, 5) < 0) 
    {
        LOG_ERROR << "Can't start listening on " << port;
    }

    LOG_INFO << "Started socket bus";

    struct sockaddr_in clientaddr;
    socklen_t clientlen = static_cast<socklen_t>(sizeof(clientaddr));
    while (1)
    {
        int childfd = accept(parentfd, (struct sockaddr *)&clientaddr, &clientlen);
        if (childfd < 0) {
            LOG_ERROR << "Can't accept new slave";
        }

        struct hostent *hostp = ::gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
                                              sizeof(clientaddr.sin_addr.s_addr), AF_INET);
        if (hostp == NULL) {
            LOG_WARNING << "Can't get slave address";
        }
        char *hostaddrp = inet_ntoa(clientaddr.sin_addr);
        if (hostaddrp == NULL) {
            LOG_WARNING << "ERROR on inet_ntoa";
        }

        if (hostp != NULL && hostaddrp) {
            LOG_INFO << "Got connection from " << hostp->h_name << " addr: " << hostaddrp;
        } else  {
            LOG_INFO << "Got new slave connection!";
        }

        static const HermesSerial empty;

        newConnection(std::make_shared<proto::UNIXSocketIO>(childfd), empty);
    }
}
