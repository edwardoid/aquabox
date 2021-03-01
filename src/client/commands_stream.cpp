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

#include "commands_stream.h"
#include "log.h"
#include "command.h"
#include "command_executor.h"

using namespace aquabox;

CommandsStream::CommandsStream(const std::string& url)
    : CloudStream(url)
{}

nlohmann::json CommandsStream::messageReceived(const nlohmann::json& message)
{
    try {
        Command cmd;
        cmd.fromJSON(message);

        CommandExecutor exe;
        if(exe.exec(cmd)) {
        }

        return cmd.response();
    } catch (nlohmann::json::parse_error err) {
        return nlohmann::json({ "error", err.what()}).dump(4);
    } catch (std::exception err) {
        return nlohmann::json({ "error", err.what()}).dump(4);
    } catch (...) {
        return nlohmann::json({ "error", "Something went wrong"}).dump(4);
    }
}