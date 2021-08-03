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

#include "events_listener.h"
#include "log.h"
#include <grpcpp/channel.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <list>
#include <functional>
#include "singleton.h"
#include "../micro_clients/settings.h"

using namespace aquabox;

template<>
const uint32_t Service<aquabox::EventsBus>::Id = 1;

EventsBus::~EventsBus()
{
    ::google::protobuf::Empty e;
    services::ConsumerIdentifier id;
    id.set_interface(m_identifier);
    if(!m_bus.call(&aquabox::services::EventsBus::Stub::Unsubscribe, id, &e).ok()) {
        LOG_ERROR << "Unsubscribing from EventBus failed!";
    }
};

EventsBus::EventsBus(const std::string& identifier)
    : m_identifier(identifier)
    , m_bus(Singleton<ISettings>::i()->port(services::EventsBus::service_full_name()))
{
    ::google::protobuf::Empty e;
    services::ConsumerIdentifier id;
    id.set_interface(m_identifier);
    m_bus.call(&aquabox::services::EventsBus::Stub::Subscribe, id, &e);

    m_server = std::thread([this] () {
        std::string server_address("0.0.0.0:");
        std::stringstream ss;
        ss << m_identifier << "/" << services::EventConsumer::service_full_name();
        server_address += std::to_string(Singleton<ISettings>::i()->port(ss.str(), true));

        Consumer service(this);

        grpc::ServerBuilder builder;
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
        std::cout << "Attaching to EventsBus(as server) as on " << server_address << std::endl;
        server->Wait();
    });

    m_client = std::thread([this] () {
        services::EventData e;
        services::ConsumerIdentifier id;
        id.set_interface(m_identifier);
        std::list<services::EventData> events;
        while(true) {
            std::unique_lock<std::mutex> lock(m_hasUpdateMx);
            m_hasUpdateCv.wait(lock);
            std::cout << "Requesting updates" << std::endl;
            grpc::ClientContext cxt;
            std::unique_ptr<grpc::ClientReader<services::EventData> > reader(m_bus->GetUpdates(&cxt, id));
            while (reader->Read(&e)) {
                events.push_back(e);
            }
            grpc::Status status = reader->Finish();
            std::cout << "Got all events" << std::endl;
        }
        for(auto& e : events) {
            auto handlers = m_callbacks.equal_range(e.sender());
            while(handlers.first != handlers.second) {
              handlers.first->second(e);
              ++handlers.first;
            }
        }
    });
}

bool EventsBus::interested(const std::string& sender)
{
    if (m_callbacks.find(sender) != m_callbacks.cend()) {
      std::unique_lock<std::mutex> lock(m_hasUpdateMx);
      m_hasUpdateCv.notify_one();
      return true;
    }
    return false;
}

void EventsBus::on(const std::string sender, std::function<bool (const ::services::EventData&)> callback)
{
    m_callbacks.insert({sender, callback});
}

void EventsBus::notify(std::string event)
{
    std::cout << "Notifying BUS about" << event << std::endl;
    services::EventData e;
    e.set_sender(m_identifier);
    e.set_event(event);
    ::google::protobuf::Empty empty;
    grpc::ClientContext cxt;
    if(m_bus->Notify(&cxt, e, &empty).ok()) {
        LOG_INFO << "Sent notification 'event'";
    }
}
