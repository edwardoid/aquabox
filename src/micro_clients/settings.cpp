/*
 * AquaBox
 * Copyright (C) 2019-2022  Eduard Sargsyan
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
#include "micro_service.h"
#include <types/serial.h>
#include <chrono>
#include <settings.grpc.pb.h>

using namespace aquabox;
using namespace aquabox::services;

template<typename T, class ClockT = std::chrono::system_clock>
class CachedSettings : protected std::unordered_map<std::string, T>
{
public:
    CachedSettings(std::chrono::milliseconds timeout = std::chrono::seconds(60))
        : m_timeout(timeout)
        , m_lastTouch(ClockT::now() - timeout)
    {}
    bool get(const std::string& key, T& value)
    {
        if ((ClockT::now() - m_lastTouch) > m_timeout)
            return false;
        if (std::unordered_map<std::string, T>::count(key) == 0)
            return false;
        value = (*this)[key];
        return true;
    }
    void set(const std::string& key, const T& value)
    {
        m_lastTouch = ClockT::now();
        (*this)[key] = value;
    }

private:
    std::chrono::milliseconds m_timeout;
    typename ClockT::time_point m_lastTouch;
};

class SettingsImpl : public ISettings
{
public:
    SettingsImpl();
    virtual ~SettingsImpl() {}
    virtual bool has(const std::string& key) override;
    virtual std::int32_t get(const std::string& key, std::int32_t defaultValue) override;
    virtual void set(const std::string& key, std::int32_t value) override;

    virtual std::string get(const std::string& key, const std::string& defaultValue) override;
    virtual void set(const std::string&, const std::string& value) override;

    virtual bool get(const std::string& key, bool defaultValue) override;
    virtual void set(const std::string&, bool value) override;

    virtual std::uint32_t port(const std::string& service, bool request = false) override;
    virtual boost::filesystem::path path(const std::string& key, const boost::filesystem::path& defaultVal) override;

private:
    services::MicroService<services::SettingsProvider, 0> m_client;
    CachedSettings<std::int32_t> m_ints;
    CachedSettings<std::uint32_t> m_uints;
    CachedSettings<std::string> m_strings;
    CachedSettings<bool> m_bools;
};

SettingsImpl::SettingsImpl()
    : m_client(500500)
{
    set_serial(get(Settings::Keys::SERIAL, Settings::Defaults::SERIAL));
}

bool SettingsImpl::has(const std::string& key)
{
    google::protobuf::StringValue _key;
    _key.set_value(key);
    google::protobuf::BoolValue val;
    if (m_client.call(&services::SettingsProvider::Stub::Has, _key, &val).ok()) {
        return val.value();
    }
    return false;
}

std::int32_t SettingsImpl::get(const std::string& key, std::int32_t defaultValue)
{
    if (!m_ints.get(key, defaultValue)) {
        google::protobuf::StringValue _key;
        _key.set_value(key);
        google::protobuf::Int32Value val;
        if (m_client.call(&services::SettingsProvider::Stub::GetInt, _key, &val).ok()) {
            m_ints.set(key, val.value());
            return val.value();
        }
        else {
            set(key, defaultValue);
        }
    }
    return defaultValue;
}

void SettingsImpl::set(const std::string& key, std::int32_t value)
{
    services::SettingsSetIntRequest req;
    google::protobuf::Empty e;
    req.set_key(key);
    req.set_value(value);
    if (m_client.call(&services::SettingsProvider::Stub::SetInt, req, &e).ok()) {
        LOG_DEBUG << "Changing " << key << " to " << value;
        m_ints.set(key, value);
    }
}

bool SettingsImpl::get(const std::string& key, bool defaultValue)
{
    if (!m_bools.get(key, defaultValue)) {
        google::protobuf::StringValue _key;
        _key.set_value(key);
        google::protobuf::BoolValue val;
        grpc::Status result = m_client.call(&services::SettingsProvider::Stub::GetBool, _key, &val);
        if (result.ok()) {
            m_uints.set(key, val.value());
            return val.value();
        }
        else {
            LOG_DEBUG << "Can't get property " << key << " " << result.error_message() << " code: " << result.error_message();
            set(key, defaultValue);
        }
    }
    return defaultValue;
}

void SettingsImpl::set(const std::string& key, bool value)
{
    services::SettingsSetBoolRequest req;
    google::protobuf::Empty e;
    req.set_key(key);
    req.set_value(value);
    if (m_client.call(&services::SettingsProvider::Stub::SetBool, req, &e).ok()) {
        LOG_DEBUG << "Changing " << key << " to " << value;
        m_bools.set(key, value);
    }
}

std::string SettingsImpl::get(const std::string& key, const std::string& defaultValue)
{
    std::string val = defaultValue;
    if (!m_strings.get(key, val)) {
        google::protobuf::StringValue _key;
        _key.set_value(key);
        google::protobuf::StringValue val;
        grpc::Status result = m_client.call(&services::SettingsProvider::Stub::GetString, _key, &val);
        if (result.ok()) {
            m_strings.set(key, val.value());
            return val.value();
        }
        else {
            LOG_DEBUG << "Can't get property " << key << " " << result.error_message() << " code: " << result.error_message();
            set(key, defaultValue);
        }
    }
    return val;
}

void SettingsImpl::set(const std::string& key, const std::string& value)
{
    services::SettingsSetStringRequest req;
    google::protobuf::Empty e;
    req.set_key(key);
    req.set_value(value);
    if (m_client.call(&services::SettingsProvider::Stub::SetString, req, &e).ok()) {
        m_strings.set(key, value);
    }
}

std::uint32_t SettingsImpl::port(const std::string& service, bool request)
{
    std::uint32_t port = 0;
    if (!m_uints.get(service, port)) {
        google::protobuf::StringValue serviceName;
        serviceName.set_value(service);
        google::protobuf::UInt32Value servicePort;
        grpc::Status res;
        if (request) {
            res = m_client.call(&services::SettingsProvider::Stub::AllocatePort, serviceName, &servicePort);
        }
        else {
            res = m_client.call(&services::SettingsProvider::Stub::GetPort, serviceName, &servicePort);
        }

        if (!res.ok()) {
            LOG_ERROR << res.error_message();
            return 0;
        }

        if (servicePort.value() == 0 && !request) {
            res = m_client.call(&services::SettingsProvider::Stub::AllocatePort, serviceName, &servicePort);
        }

        if (!res.ok()) {
            LOG_ERROR << res.error_message();
            return 0;
        }
        m_uints.set(service, servicePort.value());

        return servicePort.value();
    }
    return port;
}

boost::filesystem::path SettingsImpl::path(const std::string& key, const boost::filesystem::path& defaultVal)
{
    std::string result = get(key, defaultVal.string());
    return boost::filesystem::path(result);
}

template<>
ISettings* Singleton<ISettings>::create()
{
    return new SettingsImpl();
}

/*
std::uint32_t aquabox::services::Settings::AllocatePort(const std::string
service)
{
    grpc::ClientContext cxt;
    google::protobuf::StringValue serviceName;
    serviceName.set_value(service);
    google::protobuf::UInt32Value servicePort;
    grpc::Status res = (*this)->AllocatePort(&cxt, serviceName, &servicePort);
    if (!res.ok()) {
        std::cerr << res.error_message();
        return 0;
    }

    return servicePort.value();
}

std::uint32_t aquabox::services::Settings::GetPort(const std::string service)
{
    grpc::ClientContext cxt;
    google::protobuf::StringValue serviceName;
    serviceName.set_value(service);
    google::protobuf::UInt32Value servicePort;
    grpc::Status res = (*this)->GetPort(&cxt, serviceName, &servicePort);
    if (!res.ok()) {
        std::cerr << res.error_message();
        return 0;
    }

    return servicePort.value();
}

*/