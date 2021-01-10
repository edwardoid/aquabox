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

#include <nlohmann/json.hpp>
#include <string>
#include <mutex>

namespace aquabox
{
    class Settings
    {
        Settings() = default;

        static void read();
        static void write();
        static nlohmann::json::json_pointer pointer(const std::string& key);
    public:

        struct Keys {
            static const std::string LOG_LEVEL;

            static const std::string WORKING_DIRECTORY;
            static const std::string WEB_ROOT;

            static const std::string HTTP_PORT;
            static const std::string STREAM_PORT;
            static const std::string NETWORK;
            static const std::string AP_SSID;
            static const std::string AP_PASSWORD;

            static const std::string USERS;

            static const std::string THREADS;
            static const std::string CLOUD_URL;
            static const std::string CLOUD_PORT;
            static const std::string CLOUD_WS_PORT;
            static const std::string DATABASE_URI;
            static const std::string DATABASE_NAME;
            static const std::string HW_COLLECTION_NAME;
            static const std::string APP_COLLECTION_NAME;
            static const std::string CLIENT_INTERFACE;
            static const std::string AP_INTERFACE;
            static const std::string SERIAL;
            static const std::string FRAMEBUFFER;
            static const std::string MQTT_PORT;
            static const std::string SCT_BUS_PORT;
            static const std::string DEBUG;
            static const std::string MAX_MEM;
            static const std::string TELEGRAM_BOT_API;
        };

        struct Defaults {
            static const std::string APPLICATION;
            static const std::string DESCRIPTION;
            static const std::string VERSION;
            static const std::string GIT_HASH;
            static const uint16_t VERSION_MAJOR;
            static const uint16_t VERSION_MINOR;
            static const std::string LOCK_FILE;
            static const std::string AUTHOR;
            static const std::string EMAIL;
            static const std::string ConfigurationFile;
            static const std::string AP_SSID;
            static const std::string AP_PASSWORD;
            static const std::string CLOUD_URL;
            static const uint16_t CLOUD_PORT;
            static const uint16_t CLOUD_WS_PORT;
            static const std::string DATABASE_URI;
            static const std::string DATABASE_NAME;
            static const std::string HW_COLLECTION_NAME;
            static const std::string APP_COLLECTION_NAME;
            static const std::string CLIENT_INTERFACE;
            static const std::string AP_INTERFACE;
            static const std::string SERIAL;
            static const std::string FRAMEBUFFER;
            static const std::string TELEGRAM_BOT_API;

            static const int REST_PORT;
            static const int WS_PORT;
            static const int MQTT_PORT;
            static const int SCT_BUS_PORT;
            static const bool DEBUG;
            static const int MAX_MEM;
        };

        static void set(std::string key, bool value);
        static void set(std::string key, int value);
        static void set(std::string key, double value);
        static void set(std::string key, std::string value);
        static void set(std::string key, nlohmann::json value);

        static bool boolean(std::string key, bool def);
        static int integer(std::string key, int def);
        static double floating(std::string key, double value);
        static std::string string(std::string key, std::string def);
        static bool has(std::string key);

        static std::vector<nlohmann::json> array(std::string key);

        static void setDomain(std::string domainName) {
            domain = domainName;
            read();
        }
    private:
        static bool dataToWrite;
        static bool ready;
        static std::recursive_mutex mx;
        static nlohmann::json json;
        static std::string domain;
    };
}
