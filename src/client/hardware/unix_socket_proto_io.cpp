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

#include "unix_socket_proto_io.h"

#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <chrono>
#include <thread>
#include <log.h>

using namespace aquabox;
using namespace aquabox::proto;

UNIXSocketIO::UNIXSocketIO(int sc)
{
    m_sfd = sc;
    
    int flags = fcntl(m_sfd, F_GETFL, 0);
    fcntl(m_sfd, F_SETFL, flags | O_NONBLOCK);

    m_good = true;
    m_thread = std::thread([this]{ routine(); });
    m_thread.detach();
}

void UNIXSocketIO::close()
{
    LOG_DEBUG << "Closing socket ";
    ::close(m_sfd);
}

buffer_length_t UNIXSocketIO::wait(buffer_length_t length) const
{
    while (available() < length)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return available();
}

buffer_length_t UNIXSocketIO::available() const
{
    byte_t tmp[128];
    while(1) {
        int count = ::read(m_sfd, tmp, 128 * sizeof(byte_t));
        if (count > 0) {
            m_buffer.insert(m_buffer.end(), tmp, tmp + count);
        } else {
            break;
        }
        
    }
    return m_buffer.size();
}

buffer_length_t UNIXSocketIO::write(const byte_t *buffer, buffer_length_t sz)
{

    // virtual size_t write(const uint8_t *buf, size_t size) override;
    int count = ::write(m_sfd, buffer, (size_t)sz);
    return count > 0 ? count : 0;
}

buffer_length_t UNIXSocketIO::read(buffer_length_t sz, byte_t *buffer)
{
    sz = sz < m_buffer.size() ? sz : m_buffer.size();
    
    if (sz > m_buffer.size()) {
        size_t remaining = sz - m_buffer.size();
        if (wait(remaining) != remaining) {
            return 0;
        }
    }
    
    memcpy(buffer, m_buffer.data(), sz);
    m_buffer.erase(m_buffer.begin(), m_buffer.begin() + sz);
    
    return sz;
}

bool UNIXSocketIO::makeRequest(const proto::Message& req, proto::Message& rsp)
{
    if (m_good)
	{
		std::lock_guard<std::mutex> lock(m_ioMx);
		m_req = req;
		m_ioRequestedCV.notify_one();
		std::unique_lock<std::mutex> ioReady(m_ioReadyMx);
		m_ioReadyCV.wait(ioReady);
		rsp = m_res;
	}

	return m_good;
}

void UNIXSocketIO::routine()
{
	while(m_good)
	{
		std::unique_lock<std::mutex> ioRequested(m_ioRequestedMx);
		m_ioRequestedCV.wait(ioRequested);
		m_good = send(m_req) && get(m_res);
		m_ioReadyCV.notify_one();
	}
}
