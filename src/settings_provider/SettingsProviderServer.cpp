#include "SettingsProviderServer.h"


#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <iostream>

using namespace aquabox;
using namespace services;

SettingsProviderServer::SettingsProviderServer(uint32_t startFrom)
    : m_lastPort(startFrom)
{
}

SettingsProviderServer::~SettingsProviderServer()
{}

::grpc::Status SettingsProviderServer::AllocatePort(::grpc::ServerContext* context, const ::google::protobuf::StringValue* request, ::google::protobuf::UInt32Value* response)
{
    std::unique_lock<std::mutex> lock(m_mx);
    m_ports[request->value()] = ++m_lastPort;
    response->set_value(m_ports[request->value()]);
    std::cout << request->value() << " -> " << m_ports[request->value()] << std::endl;

    return ::grpc::Status::OK;
}

::grpc::Status SettingsProviderServer::GetPort(::grpc::ServerContext* context, const ::google::protobuf::StringValue* request, ::google::protobuf::UInt32Value* response)
{
    std::unique_lock<std::mutex> lock(m_mx);
    response->set_value(m_ports[request->value()]);
    std::cout << request->value() << " -> " << m_ports[request->value()] << std::endl;
    return ::grpc::Status::OK;
}