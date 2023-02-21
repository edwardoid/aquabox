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
 
#include "DevicesServiceServer.h"
#include "devices_map.h"
#include "events/device_events.h"
#include "hermes/hermes_mesh_gateway.h"
#include "layer/network_layer.h"
#include "relay.h"
#include "sensor.h"
#include "step.h"
#include <common/singleton.h>
#include <micro_clients/events.h>
#include <micro_clients/micro_service.h>
#include <rpc/converters.h>
#include <services/application.h>
#include <services/settings.h>

#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <micro_clients/settings.h>
#include <sstream>
#include <fstream>

using namespace aquabox;
using namespace services;

DeviceClass getClass(const boost::filesystem::path& p)
{
    std::ifstream is(p.string());
    std::string content;
    std::string line;
    while (std::getline(is, line)) {
        content += line;
    }

    try {
        nlohmann::json data = nlohmann::json::parse(content);
        auto val = data.find("class").value();
        if (val.is_string()) {
            bool ok = false;
            auto clazz = Serializable<DeviceClass>::convert(val.get<std::string>(), &ok);
            return ok ? clazz : DeviceClass::Unknown;
        }
    }
    catch (std::exception e) {
        LOG_ERROR << "Can't determine device type in " << p << " error: " << e.what();
    }

    return DeviceClass::Unknown;
}

DevicesServiceServer::DevicesServiceServer()
{
    if (!init()) {
        LOG_ERROR << "Can't init Devices Manager";
    }
}

DevicesServiceServer::~DevicesServiceServer() {}

bool DevicesServiceServer::init()
{
    Singleton<aquabox::HermesMeshGateway>::available = !Application::app()->optionsIsSet("no-spi")
                                                       && Singleton<ISettings>::i()->get(Settings::Keys::MESH_ENABLED,
                                                                                         Settings::Defaults::MESH_ENABLED);

    boost::filesystem::path root = Singleton<ISettings>::i()->path(Settings::Keys::DATA_ROOT, Settings::Defaults::DATA_ROOT)
                                   / Singleton<ISettings>::i()->path(Settings::Keys::DEVICES_DATA_SUBROOT,
                                                                     Settings::Defaults::DEVICES_DATA_SUBROOT);

    if (!boost::filesystem::exists(root)) {
        if (!boost::filesystem::create_directory(root)) {
            LOG_ERROR << "Can't create devices directory" << root;
            return false;
        }
    }

    if (!boost::filesystem::is_directory(root)) {
        LOG_ERROR << root << " is not a directory!";
        return false;
    }

    LOG_DEBUG << "Looking for devices in " << root;

    for (auto const& file : boost::filesystem::directory_iterator(root)) {
        if (boost::filesystem::is_regular_file(file)) {
            auto p = boost::filesystem::path(file);

            if (p.extension() != ".json") {
                continue;
            }

            LOG_INFO << "Loading device: " << file;
            std::shared_ptr<Device> dev = nullptr;
            switch (getClass(p)) {
            case DeviceClass::Relay: {
                dev = std::make_shared<Relay>();
                break;
            }
            case DeviceClass::Sensor: {
                dev = std::make_shared<Sensor>();
                break;
            }
            case DeviceClass::Step: {
                dev = std::make_shared<Step>();
                break;
            }
            default: {
                LOG_ERROR << "Unknown device type in " << p;
            }
            }

            if (dev != nullptr && dev->parseFile(p)) {
                auto propertiesToRestore = dev->savedProperties(p);
                if (propertiesToRestore.is_object())
                {
                    for(auto item : propertiesToRestore.items())
                    {
                        dev->setValue(item.key(), item.value());
                    }
                }
                Singleton<DevicesMap>::i()->addDevice(dev);

                m_pool.run([dev]() {
                    bool avail = Hardware::resolve(dev->address()) != nullptr;
                    dev->setAvailable(avail);
                    if(!dev->rulesEnabled()) {
                        dev->restoreLastState();
                    }
                });
            }
        }
        else {
            LOG_ERROR << file << " is not a file!";
            return false;
        }
    }

    LOG_INFO << "Loaded " << Singleton<DevicesMap>::i()->size() << " devices";

    NetworkLayer::prepare();

    m_sensorPollingThread = std::thread([this] {
        LOG_INFO << "Sensor poiing thread started";
        while (1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            processSensorUpdates();
        }
    });
    return true;
}

::grpc::Status DevicesServiceServer::GetAll(::grpc::ServerContext* /*context*/,
                                            const ::google::protobuf::Empty* /*request*/,
                                            ::grpc::ServerWriter<::aquabox::services::DeviceData>* writer)
{
    LOG_DEBUG << "Reqested all devices!";
    Singleton<DevicesMap>::i()->for_each(
        [this, writer](const auto id, const auto dev) { writer->Write(Converter<Device, DeviceData>::convert(*dev)); });
    return ::grpc::Status::OK;
}

::grpc::Status DevicesServiceServer::Update(::grpc::ServerContext* context,
                                            const ::aquabox::services::DeviceData* request,
                                            ::google::protobuf::BoolValue* response)
{
    LOG_DEBUG << "Updating device " << request->id();
    bool found = Singleton<DevicesMap>::i()->on(request->id(), [request](const auto& id, auto& dev) {
        nlohmann::json before = *dev;
        dev->setName(request->name());
        dev->setIndex(request->index());
        dev->setRulesEnabled(request->rulesenabled());
        dev->accept(request->accepted());
        dev->setType(Serializable<DeviceType>::convert(request->type()));
        Metadata meta = dev->metadata();
        for (const auto& m : request->meta()) {
            LOG_INFO << m.first << " -> " << m.second;
            meta[m.first] = m.second;
        }
        dev->setMetadata(meta);
        boost::filesystem::path path = Singleton<ISettings>::i()->path(Settings::Keys::DATA_ROOT, Settings::Defaults::DATA_ROOT)
                                     / Singleton<ISettings>::i()->path(Settings::Keys::DEVICES_DATA_SUBROOT, Settings::Defaults::DEVICES_DATA_SUBROOT)
                                     / id;
        path.replace_extension(".json");

        if (!dev->saveToFile(path)) {
            return false;
        }

        Singleton<IEvents>::i()->notify(std::make_shared<DeviceUpdatedEvent>(dev, before));

        return true;
    });

    response->set_value(found);

    if (found) {
        return ::grpc::Status::OK;
    }

    return grpc::Status(grpc::StatusCode::NOT_FOUND, std::string("Device") + request->id() + "not found");
}

::grpc::Status DevicesServiceServer::Get(::grpc::ServerContext* context,
                                         const ::google::protobuf::StringValue* request,
                                         ::aquabox::services::DeviceData* response)
{
    LOG_DEBUG << "Requested device " << request->value();
    bool found = Singleton<DevicesMap>::i()->on(request->value(), [response](const auto& id, auto& dev) {
        *response = Converter<Device, DeviceData>::convert(*dev);
        return true;
    });

    if (found) {
        return ::grpc::Status::OK;
    }
    return grpc::Status(grpc::StatusCode::NOT_FOUND, std::string("Device") + request->value() + "not found");
}

::grpc::Status DevicesServiceServer::Set(::grpc::ServerContext* context,
                                         const ::aquabox::services::SetValueRequest* request,
                                         ::google::protobuf::BoolValue* response)
{
    LOG_DEBUG << "Setting device value " << request->deviceid();
    bool found = Singleton<DevicesMap>::i()->on(request->deviceid(), [request](const auto& id, auto& dev) {
        for (auto& p : dev->properties()) {
            if (p == request->property()) {
                nlohmann::json v = request->value();
                if (dev->setValue(p, v)) {
                    boost::filesystem::path path = Singleton<ISettings>::i()->path(Settings::Keys::DATA_ROOT, Settings::Defaults::DATA_ROOT)
                                     / Singleton<ISettings>::i()->path(Settings::Keys::DEVICES_DATA_SUBROOT, Settings::Defaults::DEVICES_DATA_SUBROOT)
                                     / request->deviceid();
                    path.replace_extension(".json");
                    if (!dev->saveToFile(path)) {
                        LOG_ERROR << "Can't save " << path.c_str();
                    }

                    Singleton<IEvents>::i()->notify(std::make_shared<ValueChangedEvent>(dev, p, dev->value(p), "user"));
                    return true;
                }
                return false;
            }
        }
        return false;
    });

    response->set_value(found);

    if (found) {
        return ::grpc::Status::OK;
    }
    return grpc::Status(grpc::StatusCode::NOT_FOUND, std::string("Device") + request->deviceid() + "not found");
}

void DevicesServiceServer::processSensorUpdates()
{
    static std::unordered_map<std::string, nlohmann::json> values;
    auto devices = Singleton<DevicesMap>::i()->ids();
    for (auto id : devices) {
        auto dev = Singleton<DevicesMap>::i()->get(id);
        if (dev == nullptr) {
            continue;
        }

        if (!dev->available()) {
            for (auto& p : dev->properties()) {
                values.erase(id + p);
            }
            continue;
        }

        auto sensor = std::dynamic_pointer_cast<AbstractSensor>(dev);
        if (sensor != nullptr) {
            for (auto& p : sensor->properties()) {
                std::string key = sensor->id() + p;
                auto newValue = sensor->value(p);
                if (values.find(key) != values.end()) {
                    if (values[key] == newValue) {
                        continue;
                    }

                    values[key] = newValue;
                }
                else {
                    values.insert({key, newValue});
                }

                Singleton<IEvents>::i()->notify(std::make_shared<ValueChangedEvent>(sensor, p, newValue, "regular_check"));
            }
        }
        auto step = std::dynamic_pointer_cast<AbstractStep>(dev);
        if (step != nullptr) {
            for (auto& p : step->properties()) {
                std::string key = step->id() + p;
                auto newValue = step->steps();
                if (values.find(key) != values.end()) {
                    if (values[key] == newValue) {
                        continue;
                    }

                    values[key] = newValue;
                }
                else {
                    values.insert({key, newValue});
                }

                Singleton<IEvents>::i()->notify(std::make_shared<ValueChangedEvent>(step, p, newValue, "regular_check"));
            }
        }
    }
}

::grpc::Status DevicesServiceServer::RulesEnabled(::grpc::ServerContext* context,
                                                  const ::google::protobuf::StringValue* request,
                                                  ::google::protobuf::BoolValue* response)
{
    LOG_DEBUG << "Checking if rules enabled on '" << request->value() << "'";
    ::grpc::Status ret;
    bool found = Singleton<DevicesMap>::i()->on<::grpc::Status>(
        request->value(),
        [request, response](const auto& id, auto& dev) {
            response->set_value(dev->rulesEnabled());
            return ::grpc::Status::OK;
        },
        ret);

    if (found) {
        return ::grpc::Status::OK;
    }
    return grpc::Status(grpc::StatusCode::NOT_FOUND, std::string("Device") + request->value() + "not found");
}

::grpc::Status DevicesServiceServer::FetchProperties(::grpc::ServerContext* context,
                                                     const ::google::protobuf::StringValue* request,
                                                     ::grpc::ServerWriter<::google::protobuf::StringValue>* writer)
{
    LOG_DEBUG << "Fetching properties for " << request->value();
    bool found = Singleton<DevicesMap>::i()->on(request->value(), [writer](const auto& id, auto& dev) {
        for (const std::string& p : dev->properties()) {
            ::google::protobuf::StringValue sv;
            sv.set_value(p);
            writer->Write(sv);
        }
    });

    if (found) {
        return ::grpc::Status::OK;
    }
    return grpc::Status(grpc::StatusCode::NOT_FOUND, std::string("Device") + request->value() + "not found");
}

::grpc::Status DevicesServiceServer::PropertyValue(::grpc::ServerContext* context,
                                                   const ::aquabox::services::GetPropertyValueRequest* request,
                                                   ::google::protobuf::DoubleValue* response)
{
    LOG_DEBUG << "Fetching property '" << request->property() << "' for " << request->deviceid();
    ::grpc::Status ret;
    bool found = Singleton<DevicesMap>::i()->on<::grpc::Status>(
        request->deviceid(),
        [request, response](const auto& id, auto& dev) {
            for (const std::string& p : dev->properties()) {
                if (p == request->property()) {
                    response->set_value(dev->value(p));
                    return ::grpc::Status::OK;
                }
            }
            return grpc::Status(grpc::StatusCode::NOT_FOUND, std::string("Property") + request->property() + "not found");
        },
        ret);

    if (found) {
        return ::grpc::Status::OK;
    }
    return grpc::Status(grpc::StatusCode::NOT_FOUND, std::string("Device") + request->deviceid() + "not found");
}
