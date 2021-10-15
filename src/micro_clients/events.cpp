/*
 * AquaBox
 * Copyright (C) 2019  Eduard Sargsyan
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

#include "events.h"
#include <memory>
#include <sstream>
#include <unordered_map>
#include <events.grpc.pb.h>
#include <grpcpp/server_builder.h>
#include <thread>
#include "micro_service.h"
#include "settings.h"

using namespace aquabox;


namespace {
std::shared_ptr<services::MicroService<services::EventsBus>> client()
{
    return std::make_shared<services::MicroService<services::EventsBus>>(
        Singleton<ISettings>::i()->port(
            services::EventsBus::service_full_name()));
}

std::string identifier()
{
    std::stringstream ss;
    ss << IEvents::APPLICATION << "@" << services::EventsBus::service_full_name();
    return ss.str();
}

std::uint32_t port()
{
    return Singleton<ISettings>::i()->port(identifier());
}

std::string address()
{
    std::string server_address("0.0.0.0:");
    return server_address + std::to_string(Singleton<ISettings>::i()->port(identifier(), true));
}

} // namespace

class EventsBaseImpl: public IEvents
{
public:
    EventsBaseImpl();
    virtual void subscribe(const std::string& sender, std::function<void (const std::shared_ptr<Event>)>&& handler) override;
    virtual void notify(const std::shared_ptr<const Event> event) override;
private:
    class ConsumerService final : public services::EventConsumer::Service {
    public:
        ConsumerService()
        { }
        virtual ~ConsumerService() { }
        void add(std::string service, std::function<void (const std::shared_ptr<Event>)> callback) { m_callbacks.insert({service, callback}); }
        virtual ::grpc::Status On(::grpc::ServerContext* context, const services::EventData* request, ::google::protobuf::Empty* response) override
        {
            auto handlers = m_callbacks.equal_range(request->sender());
            std::shared_ptr<Event> e = std::make_shared<Event>();
            auto json = nlohmann::json::parse(request->event());
            e->setSender(json["sender"], json["class"]);
            e->setData(json["data"]);
            e->setPropertiesUpdate(json["props"]);
            LOG_INFO << "Got event from " << request->sender();
            while (handlers.first != handlers.second) {
                handlers.first->second(e);
                ++handlers.first;
            }
            return ::grpc::Status::OK;
        }

    private:
        std::unordered_multimap<std::string, std::function<void (const std::shared_ptr<Event>)>> m_callbacks;
    };
private:
    std::thread m_consumer;
    ConsumerService m_service;
    std::unordered_map<std::string, std::function<void (const std::shared_ptr<Event>)>> m_handlers;
};

EventsBaseImpl::EventsBaseImpl()
{
    m_consumer = std::thread([this]() {
        grpc::ServerBuilder builder;
        builder.AddListeningPort(address(), grpc::InsecureServerCredentials());
        builder.RegisterService(&m_service);
        std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
        server->Wait();
    });
}

void EventsBaseImpl::subscribe(const std::string& sender, std::function<void (const std::shared_ptr<Event>)>&& handler)
{
    services::ConsumerIdentifier id;
    id.set_interface(IEvents::APPLICATION);
    id.set_instance(identifier());
    id.set_address(address());

    google::protobuf::Empty e;

    if (client()->call(&services::EventsBus::Stub::Subscribe, id, &e).ok()) {
        m_service.add(sender, handler);
    } else {
        LOG_ERROR << "Can't subscribe for events";
    }
}

void EventsBaseImpl::notify(const std::shared_ptr<const Event> event)
{
    services::EventData e;
    e.set_sender(IEvents::APPLICATION);
    e.set_event(event->serialize().dump());

    google::protobuf::Empty v;
    if (!client()->call(&services::EventsBus::Stub::Notify, e, &v).ok()) {
        LOG_ERROR << "Can't publish update " << event->serialize().dump();
    }
}


template<>
IEvents* Singleton<IEvents>::create()
{
    return new EventsBaseImpl();
}
