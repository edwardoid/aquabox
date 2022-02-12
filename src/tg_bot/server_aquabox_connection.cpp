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

#include "server_aquabox_connection.h"
#include <client_http.hpp>
#include <common/log.h>
#include <micro_clients/settings.h>
#include <sstream>

using namespace aquabox;

ServerAquaboxConnection::ServerAquaboxConnection(const std::string id)
    : m_id(id)
{}
/*

const std::string& ServerAquaboxConnection::id() const
{
    return m_id;
}
*/

void ServerAquaboxConnection::runCommand(Command cmd, std::function<void(const Command&, bool)>&& onResult)
{
    static std::string baseUrl;
    if (baseUrl.empty()) {
        baseUrl = Singleton<ISettings>::i()->get(Settings::Keys::CLOUD_URL, Settings::Defaults::CLOUD_URL);
        baseUrl += std::string(":");
        baseUrl += std::to_string(Singleton<ISettings>::i()->port(Settings::Keys::CLOUD_PORT, Settings::Defaults::CLOUD_PORT));
    }
    SimpleWeb::Client<SimpleWeb::HTTP> client(baseUrl);

    // Asynchronous request example
    client.request("GET",
                   "/api/v1/devices",
                   cmd.responseJson().dump(),
                   {{"boxId", m_id}, {"appId", "aquaboxTgBot"}},
                   [cmd, onResult](std::shared_ptr<SimpleWeb::Client<SimpleWeb::HTTP>::Response> response, const SimpleWeb::error_code& ec) {
                       try {
                           std::stringstream ss;
                           ss << response->content.rdbuf();
                           LOG_DEBUG << "API response" << ss.str();
                           (const_cast<Command&>(cmd)).response(nlohmann::json::parse(ss.str()), Result::Ok);
                           onResult(cmd, true);
                       }
                       catch (...) {
                           (const_cast<Command&>(cmd)).response(Result::ServerError);
                           onResult(cmd, false);
                       }
                   });

    client.io_service->run();
}

void ServerAquaboxConnection::runCommand(Command cmd, MessageFormatter&& onResult)
{
    runCommand(cmd, [&onResult](const Command& cmd, bool ok) { onResult(cmd, ok); });
}