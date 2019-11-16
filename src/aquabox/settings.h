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

#ifndef SETTINGS_H
#define SETTINGS_H

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

            static int REST_PORT;
            static int WS_PORT;
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
    private:
        static bool dataToWrite;
        static bool ready;
        static std::recursive_mutex mx;
        static nlohmann::json json;
    };
}


#endif // SETTINGS_H