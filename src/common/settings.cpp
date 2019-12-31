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

#include "settings.h"
#include <fstream>
#include <thread>

#ifndef AB_GIT_HASH
#define AB_GIT_HASH "0x"
#endif

#ifndef AB_VERSION_MAJOR
#define AB_VERSION_MAJOR 1
#endif

#ifndef AB_VERSION_MINOR
#define AB_VERSION_MINOR 0
#endif

#ifndef AB_VERSION
#define AB_VERSION "1.0"
#endif

using namespace aquabox;

const std::string Settings::Keys::LOG_LEVEL = "log";

const std::string Settings::Keys::WORKING_DIRECTORY = "workingDirectory";
const std::string Settings::Keys::WEB_ROOT = "webRoot";

const std::string Settings::Keys::HTTP_PORT = "httpPort";
const std::string Settings::Keys::STREAM_PORT = "streamPort";
const std::string Settings::Keys::NETWORK = "network";
const std::string Settings::Keys::AP_SSID = "ssid";
const std::string Settings::Keys::AP_PASSWORD = "password";
const std::string Settings::Keys::USERS = "users";
const std::string Settings::Keys::THREADS = "threads";
const std::string Settings::Keys::CLOUD_URL = "cloudUrl";
const std::string Settings::Keys::CLOUD_PORT = "cloudPort";

const std::string Settings::Defaults::GIT_HASH = AB_GIT_HASH ;
const std::string Settings::Defaults::APPLICATION = "GoAqua";
const std::string Settings::Defaults::DESCRIPTION = "Some kind of smart aquarium controller";
const std::string Settings::Defaults::VERSION = AB_VERSION;
const uint16_t Settings::Defaults::VERSION_MAJOR = AB_VERSION_MAJOR;
const uint16_t Settings::Defaults::VERSION_MINOR = AB_VERSION_MINOR;
const std::string Settings::Defaults::LOCK_FILE = "/tmp/" + Settings::Defaults::APPLICATION + ".lock";
const std::string Settings::Defaults::AUTHOR = "Edward Sargsyan";
const std::string Settings::Defaults::EMAIL = "edward.sarkisyan@gmail.com";
const std::string Settings::Defaults::ConfigurationFile = "config.json";
const std::string Settings::Defaults::CLOUD_URL = "edwardoid.redirectme.net";
const uint16_t Settings::Defaults::CLOUD_PORT = 5555;
int Settings::Defaults::REST_PORT = 1213;
int Settings::Defaults::WS_PORT = 1214;
const std::string Settings::Defaults::AP_SSID = "AquaBox";
const std::string Settings::Defaults::AP_PASSWORD = "aquabox";


bool Settings::dataToWrite = false;
bool Settings::ready = false;
std::recursive_mutex Settings::mx;
nlohmann::json Settings::json;

void Settings::read()
{
    std::lock_guard<std::recursive_mutex> lock(mx);

    std::ifstream is(Defaults::ConfigurationFile);
    std::string content;
    std::string line; 
    while(std::getline(is, line))
        content += line + "\n";
    if(!content.empty())
        json = json.parse(content);
    ready = true;
}

void Settings::write()
{
    std::lock_guard<std::recursive_mutex> lock(mx);

    if (!dataToWrite)
        return;
    std::ofstream os(Settings::Defaults::ConfigurationFile);
    os << json.dump(4);
    dataToWrite = false;
}

void Settings::set(std::string key, bool value)
{
    set(key, nlohmann::json(value));
}

void Settings::set(std::string key, int value)
{
    set(key, nlohmann::json(value));
}

void Settings::set(std::string key, double value)
{
    set(key, nlohmann::json(value));
}

void Settings::set(std::string key, std::string value)
{
    set(key, nlohmann::json(value));
}

void Settings::set(std::string key, nlohmann::json value)
{
    std::lock_guard<std::recursive_mutex> lock(mx);
    json[key] = value;
    dataToWrite = true;
    write();
}

bool Settings::boolean(std::string key, bool def)
{
    std::lock_guard<std::recursive_mutex> lock(mx);
    auto pos = json.find(key);
    if (pos != json.end())
    {
        if (pos->is_boolean())
            return pos->get<bool>();
    }
}

int Settings::integer(std::string key, int def)
{
    std::lock_guard<std::recursive_mutex> lock(mx);
    if (!ready)
        read();
    auto pos = json.find(key);
    if (pos != json.end())
    {
        if (pos->is_number())
            return pos->get<int>();
    }

    set(key, def);
    return def;
}

double Settings::floating(std::string key, double value)
{
    std::lock_guard<std::recursive_mutex> lock(mx);
}

std::string Settings::string(std::string key, std::string def)
{
    std::lock_guard<std::recursive_mutex> lock(mx);
    if (!ready)
        read();
    auto pos = json.find(key.c_str());
    if (pos != json.end())
    {
        if (pos->is_string())
            return pos->get<std::string>();
    }

    set(key, def);
    return def;
}

std::vector<nlohmann::json> Settings::array(std::string key)
{
    std::lock_guard<std::recursive_mutex> lock(mx);
    if (!ready)
        read();
    auto pos = json.find(key.c_str());
    if (pos != json.end())
    {
        if (pos->is_array())
            return pos->get<std::vector<nlohmann::json>>();
    }

    set(key, nlohmann::json::array());
    return {};

}

bool Settings::has(std::string key)
{
    std::lock_guard<std::recursive_mutex> lock(mx);
}