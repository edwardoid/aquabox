#include "DevicesServiceServer.h"
#include "../micro_clients/micro_service.h"
#include "../rpc/converters.h"
#include "../types/device.h"

#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <sstream>

using namespace aquabox;
using namespace services;

DevicesServiceServer::DevicesServiceServer()
    : m_eventsBus(DevicesService::service_full_name())
{
    m_sensorPollingThread = std::thread([this] {
        while(1) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            m_eventsBus.notify("Device added");
        }
    });
}

DevicesServiceServer::~DevicesServiceServer()
{}

::grpc::Status DevicesServiceServer::GetAll(::grpc::ServerContext* /*context*/, const ::google::protobuf::Empty* /*request*/, ::grpc::ServerWriter< ::aquabox::services::DeviceData>* writer)
{
    LOG_DEBUG << "Reqested all devices!";
    std::vector<Device> data;
    data.resize(11);
    std::for_each(data.begin(), data.end(), [&writer](const auto& d) {
        writer->Write(Converter<Device, DeviceData>::convert(d));
    });
    return ::grpc::Status::OK;
}