#include "NetworkManagerServer.h"

#include <nmdbus/NetworkManager.h>

using namespace aquabox;
using namespace services;

NetworkManagerServer::NetworkManagerServer()
    : m_nm(std::make_unique<nm::NetworkManager>())
{}

NetworkManagerServer::~NetworkManagerServer()
{}

::grpc::Status NetworkManagerServer::GetCurrentNetwork(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::aquabox::services::NetworkInfo* response)
{
    return ::grpc::Status::OK;
}

::grpc::Status NetworkManagerServer::IsConnectedToInternet(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::google::protobuf::BoolValue* response)
{
    response->set_value(m_nm->getConnectivity() == nm::ConnectivityState::Full);
    return ::grpc::Status(grpc::StatusCode::ALREADY_EXISTS, "Connectivity checked!", "Check details");
}

::grpc::Status NetworkManagerServer::Connect(::grpc::ServerContext* context, const ::aquabox::services::NetworkConnectRequest* request, ::grpc::ServerWriter< ::aquabox::services::ConnectionStatus>* writer)
{
    
    return ::grpc::Status::OK;
}

::grpc::Status NetworkManagerServer::StartHotspot(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::google::protobuf::Empty* response)
{
    return ::grpc::Status::OK;
}

::grpc::Status NetworkManagerServer::StopHotspot(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::google::protobuf::Empty* response)
{
    return ::grpc::Status::OK;
}

::grpc::Status NetworkManagerServer::CurrentNetworkChanged(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::grpc::ServerWriter< ::aquabox::services::NetworkInfo>* writer)
{
    return ::grpc::Status::OK;
}
