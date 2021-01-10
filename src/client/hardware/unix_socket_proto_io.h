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

#pragma once

#include <message_io.h>
#include <vector>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace aquabox
{
    class UNIXSocketIO : public proto::MessageIO
    {
    public:
        UNIXSocketIO(int sc);
        virtual proto::buffer_length_t wait(proto::buffer_length_t length) const override;
        virtual proto::buffer_length_t available() const override;
        virtual proto::buffer_length_t write(const proto::byte_t *buffer, proto::buffer_length_t sz) override;
        virtual proto::buffer_length_t read(proto::buffer_length_t sz, proto::byte_t *buffer) override;
        virtual void close();

        virtual bool makeRequest(const proto::Message& req, proto::Message& rsp);
    private:
        void routine();
    private:
        int m_sfd;
        mutable std::vector<proto::byte_t> m_buffer;

        std::thread m_thread;

        std::mutex m_ioMx;

        std::mutex m_ioRequestedMx;
        std::mutex m_ioReadyMx;

        std::condition_variable m_ioRequestedCV;
        std::condition_variable m_ioReadyCV;

        proto::Message m_req;
        proto::Message m_res;
        bool m_good;
    };
} // namespace aquabox