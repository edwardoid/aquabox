#include "NetworkManagerServer.h"

#include "network_manager.h"
#include <services/application.h>
#include <micro_clients/settings.h>
#include <micro_clients/events.h>

using namespace aquabox;
using namespace services;

std::string security_to_string(nm::Security sec)
{
    switch(sec)
    {
        case nm::Security::WEP: return "wep";
        case nm::Security::WPA: return "wpa";
        case nm::Security::WPA2: return "wpa2";
        case nm::Security::None: return "none";
        case nm::Security::Enterprise: return "enterprise";
    }
    return "unknown";
}


NetworkManagerServer::NetworkManagerServer()
{
    if (Application::app()->optionsIsSet("ignore-networking") || Application::app()->optionsIsSet("in"))
    {
        Singleton<aquabox::NetworkManager>::available = false;
    } else {
        aquabox::NetworkManager* nm = Singleton<aquabox::NetworkManager>::i();
        nm->setInterface(Settings::string(Settings::Keys::CLIENT_INTERFACE,
            Settings::Defaults::CLIENT_INTERFACE));
        nm->currentConnection.connect([this](Connection c) {
            Singleton<IEvents>::i()->notify(std::make_shared<NetworkChangedEvent>(c));
        });
        LOG_DEBUG << "Signal id: "
                  << nm->deviceState.connect([this](NetworkDeviceState state) {
                        Singleton<IEvents>::i()->notify(std::make_shared<NetworkDeviceStateEvent>(state));
                     });
    }
}

NetworkManagerServer::~NetworkManagerServer() { }

::grpc::Status NetworkManagerServer::GetCurrentNetwork(
    ::grpc::ServerContext* context, const ::google::protobuf::Empty* request,
    ::aquabox::services::NetworkInfo* response)
{
    LOG_INFO << "IP: " << Singleton<aquabox::NetworkManager>::i()->currentConnection->ip;
    response->set_ssid(Singleton<aquabox::NetworkManager>::i()->currentConnection->ssid);
    response->set_ip(Singleton<aquabox::NetworkManager>::i()->currentConnection->ip);
    response->set_strength(Singleton<aquabox::NetworkManager>::i()->currentConnection->signal);
    response->set_auth(security_to_string(Singleton<aquabox::NetworkManager>::i()->currentConnection->auth));
    response->set_mode(Singleton<aquabox::NetworkManager>::i()->currentConnection->encrypted ? ::aquabox::services::NetworkInfo::Secure
                                                                                             : ::aquabox::services::NetworkInfo::Unsecure );
    return ::grpc::Status::OK;
}

::grpc::Status NetworkManagerServer::IsConnectedToInternet(
    ::grpc::ServerContext* context, const ::google::protobuf::Empty* request,
    ::google::protobuf::BoolValue* response)
{
    LOG_INFO << __func__;
    aquabox::NetworkManager* nm = Singleton<aquabox::NetworkManager>::i();
    response->set_value(nm != nullptr && nm->connectedToInternet());
    return ::grpc::Status(grpc::StatusCode::ALREADY_EXISTS,
        "Connectivity checked!", "Check details");
}

::grpc::Status NetworkManagerServer::Connect(
    ::grpc::ServerContext* context,
    const ::aquabox::services::NetworkConnect* request,
    ::aquabox::services::ConnectionStatus* response)
{
    LOG_INFO << __func__;
    return ::grpc::Status::OK;
}

::grpc::Status
NetworkManagerServer::StartHotspot(::grpc::ServerContext* context,
    const ::google::protobuf::Empty* request,
    ::google::protobuf::Empty* response)
{
    return ::grpc::Status::OK;
}

::grpc::Status
NetworkManagerServer::StopHotspot(::grpc::ServerContext* context,
    const ::google::protobuf::Empty* request,
    ::google::protobuf::Empty* response)
{
    return ::grpc::Status::OK;
}

::grpc::Status NetworkManagerServer::CurrentNetworkChanged(
    ::grpc::ServerContext* context, const ::google::protobuf::Empty* request,
    ::grpc::ServerWriter<::aquabox::services::NetworkInfo>* writer)
{
    return ::grpc::Status::OK;
}
