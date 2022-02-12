#include "AquaboxClientServer.h"

#include <common/singleton.h>
#include <devices.grpc.pb.h>
#include <micro_clients/settings.h>
#include <network.grpc.pb.h>
#include <rules.grpc.pb.h>

#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <iostream>

using namespace aquabox;
using namespace services;

AquaboxClientServer::AquaboxClientServer()
{
    m_rules = std::make_unique<MicroService<services::RulesService>>(Singleton<ISettings>::i()->port(RulesService::service_full_name()));
    m_devices = std::make_unique<MicroService<services::DevicesService>>(
        Singleton<ISettings>::i()->port(DevicesService::service_full_name()));
    m_network = std::make_unique<MicroService<services::NetworkManager>>(
        Singleton<ISettings>::i()->port(NetworkManager::service_full_name()));
}

AquaboxClientServer::~AquaboxClientServer() {}

::grpc::Status AquaboxClientServer::devices(::grpc::ServerContext* context,
                                            const ::google::protobuf::Empty* request,
                                            ::grpc::ServerWriter<DeviceData>* writer)
{
    grpc::ClientContext cxt;
    std::unique_ptr<grpc::ClientReader<services::DeviceData>> reader((*m_devices)->GetAll(&cxt, *request));
    DeviceData dev;
    while (reader->Read(&dev)) {
        writer->Write(dev);
    }
    grpc::Status status = reader->Finish();

    if (status.ok()) {
        LOG_DEBUG << "Call to RulesService::Get succeded";
    }
    else {
        LOG_ERROR << "Failed to invoke Call to DevicesService::GetAll with code " << status.error_code()
                  << "Message: " << status.error_message() << "Details: " << status.error_details();
    }
    return status;
}

::grpc::Status AquaboxClientServer::rules(::grpc::ServerContext* context,
                                          const ::google::protobuf::Empty* request,
                                          ::grpc::ServerWriter<RuleData>* writer)
{
    grpc::ClientContext cxt;
    std::unique_ptr<grpc::ClientReader<services::RuleData>> reader((*m_rules)->GetAll(&cxt, *request));
    RuleData rule;
    while (reader->Read(&rule)) {
        writer->Write(rule);
    }
    grpc::Status status = reader->Finish();

    if (status.ok()) {
        LOG_DEBUG << "Call to RulesService::Get succeded";
    }
    else {
        LOG_ERROR << "Failed to invoke Call to RulesService::GetAll with code " << status.error_code()
                  << "Message: " << status.error_message() << "Details: " << status.error_details();
    }
    return status;
}

::grpc::Status AquaboxClientServer::device(::grpc::ServerContext* context, const ::google::protobuf::StringValue* id, DeviceData* response)
{
    grpc::Status status = m_devices->call(&aquabox::services::DevicesService::Stub::Get, *id, response);
    if (status.ok()) {
        LOG_DEBUG << "Call to DevicesService::Get succeded";
    }
    else {
        LOG_ERROR << "Failed to invoke Call to DevicesService::Get with code " << status.error_code()
                  << "Message: " << status.error_message() << "Details: " << status.error_details();
    }
    return status;
}

::grpc::Status AquaboxClientServer::authorize(::grpc::ServerContext* context,
                                              const ::google::protobuf::StringValue* request,
                                              ::google::protobuf::BoolValue* response)
{
    return grpc::Status::OK;
}

::grpc::Status AquaboxClientServer::rule(::grpc::ServerContext* context, const ::google::protobuf::StringValue* id, RuleData* response)
{
    grpc::Status status = m_rules->call(&aquabox::services::RulesService::Stub::Get, *id, response);
    if (status.ok()) {
        LOG_DEBUG << "Call to RulesService::Get succeded";
    }
    else {
        LOG_ERROR << "Failed to invoke Call to RulesService::Get with code " << status.error_code() << "Message: " << status.error_message()
                  << "Details: " << status.error_details();
    }
    return status;
}

::grpc::Status AquaboxClientServer::addRule(::grpc::ServerContext* context, const RuleData* request, ::google::protobuf::BoolValue* response)
{
    return grpc::Status::OK;
}

::grpc::Status AquaboxClientServer::updateRule(::grpc::ServerContext* context,
                                               const RuleData* request,
                                               ::google::protobuf::BoolValue* response)
{
    return grpc::Status::OK;
}

::grpc::Status AquaboxClientServer::updateDevice(::grpc::ServerContext* context,
                                                 const DeviceData* request,
                                                 ::google::protobuf::BoolValue* response)
{
    return grpc::Status::OK;
}

::grpc::Status AquaboxClientServer::scan(::grpc::ServerContext* context,
                                         const ::google::protobuf::Empty* request,
                                         ::google::protobuf::BoolValue* response)
{
    return grpc::Status::OK;
}

::grpc::Status AquaboxClientServer::networks(::grpc::ServerContext* context,
                                             const ::google::protobuf::Empty* request,
                                             ::grpc::ServerWriter<NetworkInfo>* writer)
{
    return grpc::Status::OK;
}

::grpc::Status AquaboxClientServer::connect(::grpc::ServerContext* context,
                                            const NetworkConnect* request,
                                            ::google::protobuf::BoolValue* response)
{
    return grpc::Status::OK;
}

::grpc::Status AquaboxClientServer::status(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, StatusInfo* response)
{
    return grpc::Status::OK;
}
