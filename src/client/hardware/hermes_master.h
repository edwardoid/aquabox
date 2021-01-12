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

#include <master.h>
#include <array>
#include <memory>
#include <mutex>
#include <log.h>

#include <pthread.h>

namespace aquabox
{
    struct HermesSerial: std::array<proto::byte_t, SERIAL_LEN>
    {
        HermesSerial(const proto::byte_t serial[SERIAL_LEN]);
        std::string to_string() const;
    };
    class HermesMaster: public proto::Master, public std::recursive_mutex
    {
    public:
        HermesMaster(std::shared_ptr<proto::MessageIO> io);
        bool isConnected() const;
        HermesSerial serial() const;
        inline std::shared_ptr<proto::IO> io() { return m_io; }
    private:
        std::shared_ptr<proto::MessageIO> m_io;
        std::recursive_mutex m_mx;
    };
}