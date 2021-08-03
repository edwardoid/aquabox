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
#include "micro_service.h"
#include <settings.grpc.pb.h>

using namespace aquabox;
using namespace aquabox::services;

class SettingsImpl: public ISettings
{
public:
    SettingsImpl();
    virtual ~SettingsImpl() {}
    virtual std::int32_t get(const std::string& key, std::int32_t defaultValue) override;
    virtual void set(const std::string& key, std::int32_t value) override;
    virtual std::string get(const std::string& key, const std::string defaultValue) override;
    virtual void set(const std::string&, const std::string& value) override;
    virtual std::uint32_t port(const std::string& service, bool request = false) override;
private:
    services::MicroService<services::SettingsProvider> m_client;
};

SettingsImpl::SettingsImpl()
    : m_client(500500)
{}

std::int32_t SettingsImpl::get(const std::string& key, std::int32_t defaultValue)
{
    google::protobuf::StringValue _key;
    _key.set_value(key);
    google::protobuf::Int32Value val;
    if(m_client.call(&services::SettingsProvider::Stub::GetInt, _key, &val).ok()) {
        return val.value();
    } else {
        set(key, defaultValue);
    }
    return defaultValue;
}

void SettingsImpl::set(const std::string& key, std::int32_t value)
{
    services::SettingsSetIntRequest req;
    google::protobuf::Empty e;
    req.set_key(key);
    req.set_value(value);
    if (!m_client.call(&services::SettingsProvider::Stub::SetInt, req, &e).ok()) {
        //
    }
}

std::string SettingsImpl::get(const std::string& key, const std::string defaultValue)
{
    google::protobuf::StringValue _key;
    _key.set_value(key);
    google::protobuf::StringValue val;
    if(m_client.call(&services::SettingsProvider::Stub::GetString, _key, &val).ok()) {
        return val.value();
    } else {
        set(key, defaultValue);
    }
    return defaultValue;
}

void SettingsImpl::set(const std::string& key, const std::string& value)
{
    services::SettingsSetStringRequest req;
    google::protobuf::Empty e;
    req.set_key(key);
    req.set_value(value);
    if (!m_client.call(&services::SettingsProvider::Stub::SetString, req, &e).ok()) {
        //
    }
}

std::uint32_t SettingsImpl::port(const std::string& service, bool request)
{
    google::protobuf::StringValue serviceName;
    serviceName.set_value(service);
    google::protobuf::UInt32Value servicePort;
    grpc::Status res;
    if (request) {
        res = m_client.call(&services::SettingsProvider::Stub::AllocatePort, serviceName, &servicePort);
    } else {
        res = m_client.call(&services::SettingsProvider::Stub::GetPort, serviceName, &servicePort);
    }
    
    if (!res.ok()) {
        std::cerr << res.error_message();
        return 0;
    }
    
    return servicePort.value();
}



template<>
ISettings* Singleton<ISettings>::create() { return new SettingsImpl(); }

/*
std::uint32_t aquabox::services::Settings::AllocatePort(const std::string service)
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