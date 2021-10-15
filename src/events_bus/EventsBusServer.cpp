#include "EventsBusServer.h"
#include "../micro_clients/micro_service.h"
#include <common/log.h>

#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <sstream>

using namespace aquabox;
using namespace services;

EventsBusServer::EventsBusServer()
{
}

EventsBusServer::~EventsBusServer() { }

std::string EventsBusServer::getId(::grpc::ServerContext* context) const
{
    const auto clientMetadata = context->client_metadata();
    auto it = clientMetadata.find("client_id");
    return std::string(it->second.begin(), it->second.end());
}

::grpc::Status
EventsBusServer::Notify(::grpc::ServerContext* context,
    const ::aquabox::services::EventData* request,
    ::google::protobuf::Empty* response)
{
    std::unique_lock<std::mutex> lock(m_mx);
    for(auto c : m_consumers)
    {
        if (c.first == request->sender()) {
            continue;
        }
        EventData e;
        e.CopyFrom(*request);
        m_pool.run([e, c, this]() {
            std::unique_lock<std::mutex> lock(m_mx);
            LOG_INFO << "Sending notification from " << e.sender() << " to " << c.first << " via " << c.second;
            MicroService<EventConsumer> consumer(c.second);

            grpc::ClientContext cxt;
            google::protobuf::Empty v;
            auto res = consumer->On(&cxt, e, &v);
            if (!res.ok()) {
                LOG_WARNING << "Can't notify service " << c.first << " at " << c.second << "due: " << res.error_message();
            }
        });
    }
    return ::grpc::Status::OK;
}

::grpc::Status EventsBusServer::Subscribe(
    ::grpc::ServerContext* context,
    const ::aquabox::services::ConsumerIdentifier* request,
    ::google::protobuf::Empty* response)
{
    std::unique_lock<std::mutex> lock(m_mx);
    LOG_INFO << "Client subscribed:" << request->interface();
    m_consumers[request->interface()] = request->address();
    return ::grpc::Status::OK;
}

::grpc::Status EventsBusServer::Unsubscribe(
    ::grpc::ServerContext* context,
    const ::aquabox::services::ConsumerIdentifier* request,
    ::google::protobuf::Empty* response)
{
    std::unique_lock<std::mutex> lock(m_mx);
    //m_newEnevtsAvailableCv.notify_one();
    m_consumers.erase(request->interface());
    LOG_INFO << "Client unsubscribed:" << request->interface();
    return ::grpc::Status::OK;
}
