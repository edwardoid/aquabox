#include "settings.h"
#include "config.h"
#include <fstream>
#include <thread>

using namespace aquabox;

const std::string Settings::HTTP_PORT = "httpPort";
const std::string Settings::SSID = "ssid";
const std::string Settings::WIFI_PASSWORD = "password";
const std::string Settings::WIFI_ENCRYPTION = "encryption";

bool Settings::dataToWrite = false;
bool Settings::ready = false;
std::recursive_mutex Settings::mx;
nlohmann::json Settings::json;

void Settings::read()
{
    std::lock_guard<std::recursive_mutex> lock(mx);

    std::ifstream is(Config::ConfigurationFile);
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
    std::ofstream os(Config::ConfigurationFile);
    os << json.dump(4);
    dataToWrite = false;
}

void Settings::set(std::string key, bool value)
{
    std::lock_guard<std::recursive_mutex> lock(mx);
    json[key, value];
    dataToWrite = true;
    write();
}

void Settings::set(std::string key, int value)
{
    std::lock_guard<std::recursive_mutex> lock(mx);
    json[key] = value;
    dataToWrite = true;
    write();
}

void Settings::set(std::string key, double value)
{
    std::lock_guard<std::recursive_mutex> lock(mx);
    json[key] = value;
    dataToWrite = true;
    write();
}

void Settings::set(std::string key, std::string value)
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

bool Settings::has(std::string key)
{
    std::lock_guard<std::recursive_mutex> lock(mx);
}