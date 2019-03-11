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
JsonWrapper Settings::json;

void Settings::read()
{
    std::lock_guard<std::recursive_mutex> lock(mx);

    std::ifstream is(Config::ConfigurationFile);
    std::string content;
    std::string line; 
    while(std::getline(is, line))
        content += line + "\n";
    
    json.Parse(content);
    ready = true;
}

void Settings::write()
{
    std::lock_guard<std::recursive_mutex> lock(mx);

    if (!dataToWrite)
        return;
    std::ofstream os(Config::ConfigurationFile);
    os << json.str();
    dataToWrite = false;
}

void Settings::set(std::string key, bool value)
{
    std::lock_guard<std::recursive_mutex> lock(mx);
    json.set(key, value);
    dataToWrite = true;
    write();
}

void Settings::set(std::string key, int value)
{
    std::lock_guard<std::recursive_mutex> lock(mx);
    json.set(key, value);
    dataToWrite = true;
    write();
}

void Settings::set(std::string key, double value)
{
    std::lock_guard<std::recursive_mutex> lock(mx);
    json.set(key, value);
    dataToWrite = true;
    write();
}

void Settings::set(std::string key, std::string value)
{
    std::lock_guard<std::recursive_mutex> lock(mx);
    json.set(key, value);
    dataToWrite = true;
    write();
}

bool Settings::boolean(std::string key, bool def)
{
    std::lock_guard<std::recursive_mutex> lock(mx);
}

int Settings::integer(std::string key, int def)
{
    std::lock_guard<std::recursive_mutex> lock(mx);
    if (!ready)
        read();
    auto pos = json.FindMember(key.c_str());
    if (pos != json.MemberEnd())
    {
        if (pos->value.IsInt())
            return pos->value.GetInt();
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
    auto pos = json.FindMember(key.c_str());
    if (pos != json.MemberEnd())
    {
        if (pos->value.IsString())
            return pos->value.GetString();
    }

    set(key, def);
    return def;
}

bool Settings::has(std::string key)
{
    std::lock_guard<std::recursive_mutex> lock(mx);
}

JsonWrapper& Settings::obj()
{
    std::lock_guard<std::recursive_mutex> lock(mx);
    return json;
}