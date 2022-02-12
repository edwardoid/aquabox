#include "SettingsProviderServer.h"

#include <fstream>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

using namespace aquabox;
using namespace services;

SettingsProviderServer::SettingsProviderServer(uint32_t startFrom)
    : m_lastPort(startFrom)
{
    load();
}

void SettingsProviderServer::load()
{
    std::unique_lock<std::recursive_mutex> lock(m_mx);

    std::ifstream is("config.json");
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
            //LOG_ERROR << "Can't parse settings " << e.what();
        }
    }
}

void SettingsProviderServer::save()
{
    LOG_INFO << "Saving configuration file...";
    std::lock_guard<std::recursive_mutex> lock(m_mx);

    m_data["ports"] = m_ports;
    m_data["lastPort"] = m_lastPort;
    std::ofstream os("config.json");
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
        return grpc::Status(grpc::StatusCode::NOT_FOUND, std::string("Configuration does not have") + request->value());
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
