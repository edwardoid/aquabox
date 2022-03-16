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
//#include "log.h"
#include <fstream>
#include <thread>

#ifndef AB_GIT_HASH
#define AB_GIT_HASH "0x"
#endif // AB_GIT_HASH

#ifndef AB_VERSION_MAJOR
#define AB_VERSION_MAJOR 1
#endif // AB_VERSION_MAJOR

#ifndef AB_VERSION_MINOR
#define AB_VERSION_MINOR 0
#endif // AB_VERSION_MINOR

#ifndef AB_VERSION
#define AB_VERSION "1.0"
#endif // AB_VERSION

using namespace aquabox;

std::string Settings::domain = "";
const std::string Settings::Keys::LOG_LEVEL = "log";
const std::string Settings::Keys::PERSISTENT_LOG_ENABLED = "keepLogs";
const std::string Settings::Keys::PERSISTENT_LOG_LOCATION = "logLocation";

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
const std::string Settings::Keys::CLOUD_WS_PORT = "cloudWSPort";
const std::string Settings::Keys::DATABASE_URI = "databaseURI";
const std::string Settings::Keys::DATABASE_NAME = "database";
const std::string Settings::Keys::HW_COLLECTION_NAME = "hardwareCollection";
const std::string Settings::Keys::APP_COLLECTION_NAME = "applicationsCollection";
const std::string Settings::Keys::CLIENT_INTERFACE = "interface";
const std::string Settings::Keys::AP_INTERFACE = "apInterface";
const std::string Settings::Keys::SERIAL = "serial";
const std::string Settings::Keys::FRAMEBUFFER = "framebuffer";
const std::string Settings::Keys::MQTT_PORT = "mqtt";
const std::string Settings::Keys::SCT_BUS_PORT = "sct";
const std::string Settings::Keys::DEBUG = "debug";
const std::string Settings::Keys::MAX_MEM = "maxMemory";
const std::string Settings::Keys::TELEGRAM_BOT_API = "telegramBotAPIKey";
const std::string Settings::Keys::MESH_ENABLED = "meshEnabled";
const std::string Settings::Keys::RF24_BUS = "rf24Bus";
const std::string Settings::Keys::RF24_RADIO_PIN = "rf24Pin";
const std::string Settings::Keys::DATA_ROOT = "root";
const std::string Settings::Keys::DEVICES_DATA_SUBROOT = "devices";
const std::string Settings::Keys::RULES_DATA_SUBROOT = "rules";
const std::string Settings::Keys::FPS = "fps";

const bool Settings::Defaults::PERSISTENT_LOG_ENABLED = false;
const std::string Settings::Defaults::PERSISTENT_LOG_LOCATION = "logs";
const std::string Settings::Defaults::GIT_HASH = AB_GIT_HASH;
const std::string Settings::Defaults::APPLICATION = "aquabox";
const std::string Settings::Defaults::DESCRIPTION = "Some kind of smart aquarium controller";
const std::string Settings::Defaults::VERSION = AB_VERSION;
const uint16_t Settings::Defaults::VERSION_MAJOR = AB_VERSION_MAJOR;
const uint16_t Settings::Defaults::VERSION_MINOR = AB_VERSION_MINOR;
const std::string Settings::Defaults::LOCK_FILE = "/tmp/" + Settings::Defaults::APPLICATION + ".lock";
const std::string Settings::Defaults::AUTHOR = "Edward Sargsyan";
const std::string Settings::Defaults::EMAIL = "edward.sarkisyan@gmail.com";
const std::string Settings::Defaults::ConfigurationFile = "config.json";
const std::string Settings::Defaults::CLOUD_URL = "aquabox.me";
const uint16_t Settings::Defaults::CLOUD_PORT = 5555;
const uint16_t Settings::Defaults::CLOUD_WS_PORT = 5556;
const int Settings::Defaults::REST_PORT = 1213;
const int Settings::Defaults::WS_PORT = 1214;
const std::string Settings::Defaults::AP_SSID = "AquaBox";
const std::string Settings::Defaults::AP_PASSWORD = "aquabox";
const std::string Settings::Defaults::DATABASE_URI = "mongodb://admin:password@aquabox.me";
const std::string Settings::Defaults::DATABASE_NAME = "aquabox";
const std::string Settings::Defaults::HW_COLLECTION_NAME = "hw";
const std::string Settings::Defaults::APP_COLLECTION_NAME = "app";
const std::string Settings::Defaults::CLIENT_INTERFACE = "wlan0";
const std::string Settings::Defaults::AP_INTERFACE = "wlan0";
const std::string Settings::Defaults::SERIAL = "testDev";
const std::string Settings::Defaults::FRAMEBUFFER = "/dev/fb0";
const int Settings::Defaults::MQTT_PORT = 1883;
const int Settings::Defaults::SCT_BUS_PORT = 1800;
const bool Settings::Defaults::DEBUG = false;
const int Settings::Defaults::MAX_MEM = 80;
const std::string Settings::Defaults::TELEGRAM_BOT_API = "";
const bool Settings::Defaults::MESH_ENABLED = true;
const int Settings::Defaults::RF24_BUS = 1;
const int Settings::Defaults::RF24_RADIO_PIN = 25;
const std::string Settings::Defaults::DATA_ROOT = "/root/aquabox/data";
const std::string Settings::Defaults::DEVICES_DATA_SUBROOT = "devices";
const std::string Settings::Defaults::RULES_DATA_SUBROOT = "rules";
const int Settings::Defaults::FPS = 10;

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
    while (std::getline(is, line)) {
        content += line + "\n";
    }
    if (!content.empty()) {
        try {
            json = json.parse(content);
        }
        catch (nlohmann::detail::parse_error& e) {
            //LOG_ERROR << "Can't parse settings " << e.what();
        }
    }
    ready = true;
}

void Settings::write()
{
    std::lock_guard<std::recursive_mutex> lock(mx);

    if (!ready) {
        read();
    }

    if (!dataToWrite)
        return;
    std::ofstream os(Settings::Defaults::ConfigurationFile);
    os << json.dump(4);
    dataToWrite = false;
}

nlohmann::json::json_pointer Settings::pointer(const std::string& key)
{
    nlohmann::json::json_pointer p;
    if (domain != "") {
        p.push_back(domain);
    }
    if (key != "") {
        p.push_back(key);
    }

    return p;
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
    nlohmann::json::json_pointer p = pointer(key);
    if (!json.contains(domain)) {
        json[domain] = nlohmann::json::object();
    }
    json[p] = value;
    dataToWrite = true;
    write();
}

bool Settings::boolean(std::string key, bool def)
{
    std::lock_guard<std::recursive_mutex> lock(mx);
    nlohmann::json::json_pointer p = pointer(key);
    if (json.contains(p)) {
        auto pos = json[p];
        if (json[p].is_boolean()) {
            return pos.get<bool>();
        }
    }
    set(key, def);
    return def;
}

int Settings::integer(std::string key, int def)
{
    std::lock_guard<std::recursive_mutex> lock(mx);
    nlohmann::json::json_pointer p = pointer(key);
    if (json.contains(p)) {
        auto pos = json[p];
        if (json[p].is_number_integer()) {
            return pos.get<int>();
        }
    }
    set(key, def);
    return def;
}

double Settings::floating(std::string key, double def)
{
    std::lock_guard<std::recursive_mutex> lock(mx);
    nlohmann::json::json_pointer p = pointer(key);

    if (json.contains(p)) {
        auto pos = json[p];
        if (json[p].is_number_float()) {
            return pos.get<double>();
        }
    }
    set(key, def);
    return def;
}

std::string Settings::string(std::string key, std::string def)
{
    std::lock_guard<std::recursive_mutex> lock(mx);
    nlohmann::json::json_pointer p = pointer(key);

    if (json.contains(p)) {
        auto pos = json[p];
        if (json[p].is_string()) {
            return pos.get<std::string>();
        }
    }
    set(key, def);
    return def;
}

std::vector<nlohmann::json> Settings::array(std::string key)
{
    std::lock_guard<std::recursive_mutex> lock(mx);
    nlohmann::json::json_pointer p = pointer(key);

    if (json.contains(p)) {
        auto pos = json[p];
        if (json[p].is_array()) {
            return pos.get<std::vector<nlohmann::json>>();
        }
    }

    return {};
}

bool Settings::has(std::string key)
{
    std::lock_guard<std::recursive_mutex> lock(mx);

    return json.contains(pointer(key));
}
