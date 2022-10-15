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
 
#include "SettingsProviderServer.h"

#include <fstream>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <boost/filesystem.hpp>
#include "services/settings.h"
#include "common/log.h"

using namespace aquabox;
using namespace services;

SettingsProviderServer::SettingsProviderServer(uint32_t startFrom)
    : m_lastPort(startFrom)
{
    m_json_path = (boost::filesystem::current_path() / "config.json").c_str();
    load();
}

void SettingsProviderServer::load()
{
    plog::get()->setMaxSeverity(plog::severityFromString("DEBUG"));
    std::unique_lock<std::recursive_mutex> lock(m_mx);

    LOG_INFO << "Loading configuration from " << m_json_path;

    std::ifstream is(m_json_path);
    std::string content;
    std::string line;
    while (std::getline(is, line)) {
        content += line + "\n";
    }
    if (!content.empty()) {
        try {
            m_data = m_data.parse(content);
            if (m_data.contains("ports")) {
                m_ports = m_data["ports"].get<std::unordered_map<std::string, std::uint32_t>>();
            }

            if (m_data.contains("lastPort")) {
                m_lastPort = m_data["lastPort"].get<std::uint32_t>();
            }
            LOG_INFO << "Config is ready!";
        }
        catch (nlohmann::detail::parse_error& e) {
            LOG_ERROR << "Can't parse settings " << e.what();
        }
    }

    if (m_data.contains(aquabox::Settings::Keys::LOG_LEVEL)) {
        const std::string severity = m_data[aquabox::Settings::Keys::LOG_LEVEL];
        plog::get()->setMaxSeverity(plog::severityFromString(severity.c_str()));
    } else {
        plog::get()->setMaxSeverity(plog::severityFromString("DEBUG"));
    }
}

void SettingsProviderServer::save()
{
    LOG_INFO << "Saving configuration file to " << m_json_path;
    std::lock_guard<std::recursive_mutex> lock(m_mx);

    m_data["ports"] = m_ports;
    m_data["lastPort"] = m_lastPort;
    std::ofstream os(m_json_path);
    os << m_data.dump(4);
}

SettingsProviderServer::~SettingsProviderServer() {}

::grpc::Status SettingsProviderServer::Has(::grpc::ServerContext* context,
                                           const ::google::protobuf::StringValue* request,
                                           ::google::protobuf::BoolValue* response)
{
    std::unique_lock<std::recursive_mutex> lock(m_mx);
    response->set_value(m_data[request->value()]);
    return ::grpc::Status::OK;
}

::grpc::Status SettingsProviderServer::AllocatePort(::grpc::ServerContext* context,
                                                    const ::google::protobuf::StringValue* request,
                                                    ::google::protobuf::UInt32Value* response)
{
    std::unique_lock<std::recursive_mutex> lock(m_mx);
    if (!m_ports.count(request->value())) {
        m_ports[request->value()] = ++m_lastPort;
    }
    response->set_value(m_ports[request->value()]);
    save();
    LOG_DEBUG << "Requested to allocate port for service " << request->value() << ". Assigned to " << response->value();

    return ::grpc::Status::OK;
}

::grpc::Status SettingsProviderServer::GetPort(::grpc::ServerContext* context,
                                               const ::google::protobuf::StringValue* request,
                                               ::google::protobuf::UInt32Value* response)
{
    std::unique_lock<std::recursive_mutex> lock(m_mx);
    response->set_value(m_ports[request->value()]);

    LOG_DEBUG << "Requested port for service " << request->value() << ". Assigned to " << response->value();
    return ::grpc::Status::OK;
}

::grpc::Status SettingsProviderServer::GetString(::grpc::ServerContext* context,
                                                 const ::google::protobuf::StringValue* request,
                                                 ::google::protobuf::StringValue* response)
{
    std::unique_lock<std::recursive_mutex> lock(m_mx);
    if (m_data.contains(request->value())) {
        response->set_value(m_data[request->value()]);
        LOG_DEBUG << "Value of " << request->value() << " -> " << response->value();
    }
    else {
        LOG_WARNING << "Can't find value for " << request->value();
        return grpc::Status(grpc::StatusCode::NOT_FOUND, std::string("Configuration does not have ") + request->value());
    }
    return ::grpc::Status::OK;
}

::grpc::Status SettingsProviderServer::SetString(::grpc::ServerContext* context,
                                                 const ::aquabox::services::SettingsSetStringRequest* request,
                                                 ::google::protobuf::Empty* response)
{
    std::unique_lock<std::recursive_mutex> lock(m_mx);
    LOG_DEBUG << "Updating " << request->key() << " to " << request->value();
    m_data[request->key()] = request->value();
    save();
    return ::grpc::Status::OK;
}

::grpc::Status SettingsProviderServer::GetBool(::grpc::ServerContext* context,
                                               const ::google::protobuf::StringValue* request,
                                               ::google::protobuf::BoolValue* response)
{
    std::unique_lock<std::recursive_mutex> lock(m_mx);
    if (m_data.contains(request->value())) {
        response->set_value(m_data[request->value()]);
        LOG_DEBUG << "Value of " << request->value() << " -> " << response->value();
    }
    else {
        LOG_WARNING << "Can't find value for " << request->value();
        return grpc::Status(grpc::StatusCode::NOT_FOUND, std::string("Configuration does not have") + request->value());
    }
    return ::grpc::Status::OK;
}

::grpc::Status SettingsProviderServer::SetBool(::grpc::ServerContext* context,
                                               const ::aquabox::services::SettingsSetBoolRequest* request,
                                               ::google::protobuf::Empty* response)
{
    std::unique_lock<std::recursive_mutex> lock(m_mx);
    LOG_DEBUG << "Updating " << request->key() << " to " << request->value();
    m_data[request->key()] = request->value();
    save();
    return ::grpc::Status::OK;
}

::grpc::Status SettingsProviderServer::GetInt(::grpc::ServerContext* context,
                                               const ::google::protobuf::StringValue* request,
                                               ::google::protobuf::Int32Value* response)
{
    std::unique_lock<std::recursive_mutex> lock(m_mx);
    if (m_data.contains(request->value())) {
        response->set_value(m_data[request->value()]);
        LOG_DEBUG << "Value of " << request->value() << " -> " << response->value();
    }
    else {
        LOG_WARNING << "Can't find value for " << request->value();
        return grpc::Status(grpc::StatusCode::NOT_FOUND, std::string("Configuration does not have") + request->value());
    }
    return ::grpc::Status::OK;
}

::grpc::Status SettingsProviderServer::SetInt(::grpc::ServerContext* context,
                                               const ::aquabox::services::SettingsSetIntRequest* request,
                                               ::google::protobuf::Empty* response)
{
    std::unique_lock<std::recursive_mutex> lock(m_mx);
    LOG_DEBUG << "Updating " << request->key() << " to " << request->value();
    m_data[request->key()] = request->value();
    save();
    return ::grpc::Status::OK;
}
