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

#include "cloud_appender.h"
#include "aquabox.h"
#include <thread>

using namespace aquabox;

void CloudAppender::write(const plog::Record& record)
{
    std::lock_guard<std::mutex> guard(m_mx);
    auto stream = AquaBox::i().cloud().stream("log");
    if (!stream) {
        return;
    }

    stream->send(record.getMessage());
}