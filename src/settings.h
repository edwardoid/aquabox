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

        static const std::string HTTP_PORT;
        static const std::string SSID;
        static const std::string WIFI_PASSWORD;
        static const std::string WIFI_ENCRYPTION;

        static void set(std::string key, bool value);
        static void set(std::string key, int value);
        static void set(std::string key, double value);
        static void set(std::string key, std::string value);

        static bool boolean(std::string key, bool def);
        static int integer(std::string key, int def);
        static double floating(std::string key, double value);
        static std::string string(std::string key, std::string def);
        static bool has(std::string key);
    private:
        static bool dataToWrite;
        static bool ready;
        static std::recursive_mutex mx;
        static nlohmann::json json;
    };
}


#endif // SETTINGS_H