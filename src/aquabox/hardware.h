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

#ifndef HARDWARE_H
#define HARDWARE_H

#include "types.h"
#include <memory>

namespace aquabox
{
    class Hardware
    {
    public:
        static std::shared_ptr<Hardware> resolve(const std::string& address);
        virtual bool open(int flags) = 0;
        virtual bool close() = 0;
        virtual BytesCount send(const Bytes& data, const BytesCount count = 0) = 0;
        virtual BytesCount read(Bytes& data, const Offset = 0) = 0;
    };
}

#endif // HARDWARE_H