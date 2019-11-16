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

#ifndef COMMANDS_STREAM_H
#define COMMANDS_STREAM_H

#include "cloud_stream.h"

namespace aquabox
{
    class CommandsStream: public CloudStream
    {
    public:
        CommandsStream(std::string url);
        virtual ~CommandsStream() {}
    protected:
        virtual void onMessage(std::shared_ptr<class Connection> connection, std::shared_ptr<class InMessage> message) override;
    };
}

#endif // COMMANDS_STREAM