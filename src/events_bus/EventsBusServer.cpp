#include "EventsBusServer.h"
#include "../micro_clients/micro_service.h"

#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <sstream>

using namespace aquabox;
using namespace services;

EventsBusServer::EventsBusServer()
{
    m_consumersNotifier = std::thread(std::bind(&EventsBusServer::notifyConsumers, this));
}

EventsBusServer::~EventsBusServer()
{}

std::string EventsBusServer::getId(::grpc::ServerContext* context) const
{
    const auto clientMetadata = context->client_metadata();
    auto it = clientMetadata.find("client_id");
    return std::string(it->second.begin(), it->second.end());
}

::grpc::Status EventsBusServer::Notify(::grpc::ServerContext* context, const ::aquabox::services::EventData* request, ::google::protobuf::Empty* response)
{
    std::unique_lock<std::mutex> lock(m_newEnevtsAvailableMx);
    std::cout << "Got event from" << request->sender() << " -> " << request->event() << std::endl;
    m_pendingEvents.push(*request);
    m_newEnevtsAvailableCv.notify_one();
    return ::grpc::Status::OK;
}

::grpc::Status EventsBusServer::Subscribe(::grpc::ServerContext* context, const ::aquabox::services::ConsumerIdentifier* request, ::google::protobuf::Empty* response)
{
    std::unique_lock<std::mutex> lock(m_newEnevtsAvailableMx);
    m_newEnevtsAvailableCv.notify_one();
    std::cout << "Client subscribed:" << request->interface() << std::endl;
    m_events[request->interface()];
    return ::grpc::Status::OK;
}

::grpc::Status EventsBusServer::Unsubscribe(::grpc::ServerContext* context, const ::aquabox::services::ConsumerIdentifier* request, ::google::protobuf::Empty* response)
{
    std::unique_lock<std::mutex> lock(m_newEnevtsAvailableMx);
    m_newEnevtsAvailableCv.notify_one();
    m_events.erase(request->interface());
    std::cout << "Client unsubscribed:" << request->interface() << std::endl;
    return ::grpc::Status::OK;
}

::grpc::Status EventsBusServer::GetUpdates(::grpc::ServerContext* context, const ::aquabox::services::ConsumerIdentifier* request, ::grpc::ServerWriter< ::aquabox::services::EventData>* writer)
{
    std::queue<EventData> data;
    {
        std::unique_lock<std::mutex> lock(m_newEnevtsAvailableMx);
        data = m_events[request->interface()];
        m_events[request->interface()] = {};
    }
    while(!data.empty()) {
        writer->Write(data.front());
        data.pop();
    }
    return ::grpc::Status::OK;
}

void EventsBusServer::notifyConsumers()
{
    while(1) {
        std::unique_lock<std::mutex> lock(m_newEnevtsAvailableMx);
        m_newEnevtsAvailableCv.wait(lock);
        while(!m_pendingEvents.empty()) {
            ::aquabox::services::EventData e = m_pendingEvents.front();
            m_pendingEvents.pop();
            ::aquabox::services::EventDescriptor descriptor;
            descriptor.set_sender(e.sender());
            descriptor.set_senderinstance(e.senderinstance());
            for(auto& consumer : m_events) {
                
                std::stringstream ss;
                ss << consumer.first << "/" << EventConsumer::service_full_name();
                std::string notificationServerPort = ss.str();

                MicroService<EventConsumer> c(Singleton<ISettings>::i()->port(notificationServerPort));

                grpc::ClientContext cxt;
                ::google::protobuf::BoolValue interested;
                std::cout << "Checking if " << consumer.first << " is interested ";
                if(!c->NewEventAvailable(&cxt, descriptor, &interested).ok()) {
                    continue;
                }
                
                if (interested.value()) {
                    std::cout << "yes";
                    m_events[consumer.first].push(e);
                } else {
                    std::cout << "no";
                }

                std::cout << std::endl;
            }
        }
    }
}