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

#include "devices_manager.h"
#include "micro_service.h"
#include "services.h"
#include "../rpc/converters.h"
#include "../services/settings.h"
#include "../services/service_identifier.h"

using namespace aquabox;

template<>
IDevicesManager* Singleton<IDevicesManager>::create() { return new DevicesManagerImpl(); }

namespace
{
    std::shared_ptr<services::MicroService<services::DevicesService>> client()
    {
        return std::make_shared<services::MicroService<services::DevicesService>>(Singleton<ISettings>::i()->port(services::DevicesService::service_full_name()));
    }
}

DevicesManagerImpl::DevicesManagerImpl()
{}

Device DevicesManagerImpl::get(const std::string& id)
{
    google::protobuf::StringValue _id;
    _id.set_value(id);
    services::DeviceData data;

    if(client()->call(&services::DevicesService::Stub::Get, _id, &data).ok()) {
        return services::Converter<services::DeviceData, Device>::convert(data);
    }

    return Device();
}

bool DevicesManagerImpl::update(const Device& device)
{
    google::protobuf::BoolValue res;
    services::DeviceData data = services::Converter<Device, services::DeviceData>::convert(device);
    if(client()->call(&services::DevicesService::Stub::Update, data, &res).ok()) {
        return true;
    }

    return false;
}

std::vector<Device> DevicesManagerImpl::all()
{
    grpc::ClientContext cxt;
    auto c = client();
    std::vector<Device> res;
    google::protobuf::Empty e;
    std::unique_ptr<grpc::ClientReader<services::DeviceData> > reader((*c)->GetAll(&cxt, e));
    services::DeviceData data;
    while (reader->Read(&data)) {
        res.push_back(services::Converter<services::DeviceData, Device>::convert(data));
    }
    grpc::Status status = reader->Finish();
    return res;
}
