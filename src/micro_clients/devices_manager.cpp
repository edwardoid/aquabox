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

#include "devices_manager.h"

#include "micro_service.h"
#include "services.h"
#include <devices.grpc.pb.h>
#include <rpc/converters.h>
#include <services/service_identifier.h>
#include <services/settings.h>

using namespace aquabox;

class DevicesManagerImpl : public IDevicesManager
{
public:
    DevicesManagerImpl();
    virtual ~DevicesManagerImpl() {}
    virtual DeviceRef get(const std::string& id) override;
    virtual bool has(const std::string& id) override;
    virtual bool update(const DeviceRef& revice) override;
    virtual bool set(const std::string& id, const ValueChange& value) override;
    virtual bool set(const std::string& id, const std::string& property, const nlohmann::json& value) override;
    virtual bool rulesEnabled(const std::string& id) override;
    virtual std::vector<DeviceRef> all() override;
    virtual std::vector<std::string> properties(const std::string& id) override;
    virtual nlohmann::json value(const std::string& id, const std::string& property) override;
};

template<>
IDevicesManager* Singleton<IDevicesManager>::create()
{
    return new DevicesManagerImpl();
}

namespace
{
    services::MicroService<services::DevicesService>& client()
    {
        static services::MicroService<services::DevicesService> c(
            Singleton<ISettings>::i()->port(services::DevicesService::service_full_name()));
        return c;
    }
} // namespace

DevicesManagerImpl::DevicesManagerImpl() {}

DeviceRef DevicesManagerImpl::get(const std::string& id)
{
    google::protobuf::StringValue _id;
    _id.set_value(id);
    services::DeviceData data;

    if (client().call(&services::DevicesService::Stub::Get, _id, &data).ok()) {
        return static_cast<DeviceRef>(data);
    }

    return DeviceRef();
}

bool DevicesManagerImpl::has(const std::string& id)
{
    google::protobuf::StringValue _id;
    _id.set_value(id);
    services::DeviceData data;

    return client().call(&services::DevicesService::Stub::Get, _id, &data).error_code() != grpc::StatusCode::NOT_FOUND;
}

bool DevicesManagerImpl::update(const DeviceRef& device)
{
    google::protobuf::BoolValue res;
    services::DeviceData data = static_cast<DeviceRef>(device);
    if (client().call(&services::DevicesService::Stub::Update, data, &res).ok()) {
        return true;
    }

    return false;
}

std::vector<DeviceRef> DevicesManagerImpl::all()
{
    grpc::ClientContext cxt;
    auto c = client();
    std::vector<DeviceRef> res;
    google::protobuf::Empty e;
    std::unique_ptr<grpc::ClientReader<services::DeviceData>> reader(c->GetAll(&cxt, e));
    services::DeviceData data;
    while (reader->Read(&data)) {
        res.push_back(static_cast<DeviceRef>(data));
    }
    grpc::Status status = reader->Finish();
    return res;
}

bool DevicesManagerImpl::set(const std::string& id, const ValueChange& value)
{
    return set(id, value.property, value.value);
}

bool DevicesManagerImpl::set(const std::string& id, const std::string& property, const nlohmann::json& value)
{
    google::protobuf::BoolValue res;
    ::aquabox::services::SetValueRequest req;
    req.set_deviceid(id);
    req.set_property(property);
    req.set_value(value.get<double>());
    if (client().call(&services::DevicesService::Stub::Set, req, &res).ok()) {
        return true;
    }

    return false;
}

bool DevicesManagerImpl::rulesEnabled(const std::string& id)
{
    google::protobuf::StringValue _id;
    _id.set_value(id);
    google::protobuf::BoolValue enabled;

    if (client().call(&services::DevicesService::Stub::RulesEnabled, _id, &enabled).ok()) {
        return enabled.value();
    }

    return false;
}

std::vector<std::string> DevicesManagerImpl::properties(const std::string& id)
{
    grpc::ClientContext cxt;
    auto c = client();
    std::vector<std::string> res;
    google::protobuf::StringValue req;
    req.set_value(id);
    std::unique_ptr<grpc::ClientReader<google::protobuf::StringValue>> reader(c->FetchProperties(&cxt, req));
    google::protobuf::StringValue data;
    while (reader->Read(&data)) {
        res.push_back(data.value());
    }
    grpc::Status status = reader->Finish();
    return res;
}

nlohmann::json DevicesManagerImpl::value(const std::string& id, const std::string& property)
{
    google::protobuf::DoubleValue res;
    ::aquabox::services::GetPropertyValueRequest req;
    req.set_deviceid(id);
    req.set_property(property);
    if (client().call(&services::DevicesService::Stub::PropertyValue, req, &res).ok()) {
        return nlohmann::json(res.value());
    }

    return nlohmann::json();
}
