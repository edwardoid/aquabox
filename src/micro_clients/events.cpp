/*
 * AquaBox
 * Copyright (C) 2019-2022  Eduard Sargsyan
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
#include "micro_service.h"
#include "settings.h"
#include <common/waitable_queue.h>
#include <common/calltime.h>
#include <events.grpc.pb.h>
#include <grpcpp/server_builder.h>
#include <memory>
#include <services/application.h>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>

using namespace aquabox;

namespace
{
    services::MicroService<services::EventsBus>& client()
    {
        int p = 0;
        do
        {
            p = Singleton<ISettings>::i()->port(services::EventsBus::service_full_name());

            while(p == 0)
                std::this_thread::sleep_for(std::chrono::seconds(3));
        } while(p == 0);
        
        static services::MicroService<services::EventsBus> c(p);
        return c;
    }

    std::string identifier()
    {
        std::stringstream ss;
        ss << Application::app()->identifier() << "@" << services::EventsBus::service_full_name();
        return ss.str();
    }

    std::uint32_t port() { return Singleton<ISettings>::i()->port(identifier()); }

    std::string address()
    {
        static const std::string server_address("0.0.0.0:");
        return server_address + std::to_string(Singleton<ISettings>::i()->port(identifier(), true));
    }

} // namespace

class EventsBaseImpl : public IEvents
{
public:
    EventsBaseImpl();
    virtual void subscribe(const std::string& sender, std::function<void(const std::shared_ptr<Event>)>&& handler) override;
    virtual void notify(const std::shared_ptr<const Event> event) override;

private:
    class ConsumerService final : public services::EventConsumer::Service
    {
    public:
        ConsumerService() {}
        virtual ~ConsumerService() {}
        void add(std::string service, std::function<void(const std::shared_ptr<Event>)> callback)
        {
            m_callbacks.insert({service, callback});
        }
        virtual ::grpc::Status On(::grpc::ServerContext* context,
                                  const services::EventData* request,
                                  ::google::protobuf::Empty* response) override
        {
            auto handlers = m_callbacks.equal_range(request->sender());
            std::shared_ptr<Event> e = std::make_shared<Event>();
            *e =  nlohmann::json::parse(request->event());
            LOG_INFO << "Got event from " << request->sender();
            while (handlers.first != handlers.second) {
                auto cb = handlers.first->second;
                boost::asio::post(m_pool, [cb, e](){
                    cb(e);
                });
                ++handlers.first;
            }
            return ::grpc::Status::OK;
        }

    private:
        std::unordered_multimap<std::string, std::function<void(const std::shared_ptr<Event>)>> m_callbacks;
        static boost::asio::thread_pool m_pool;
    };

private:
    std::thread m_producer;
    std::thread m_consumer;
    ConsumerService m_service;
    std::unordered_map<std::string, std::function<void(const std::shared_ptr<Event>)>> m_handlers;
    WaitableQueue<services::EventData> m_events;
};

boost::asio::thread_pool EventsBaseImpl::ConsumerService::m_pool;

EventsBaseImpl::EventsBaseImpl()
{
    m_consumer = std::thread([this]() {
        grpc::ServerBuilder builder;
        builder.AddListeningPort(address(), grpc::InsecureServerCredentials());
        builder.RegisterService(&m_service);
        std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
        server->Wait();
    });

    m_producer = std::thread([this]() {
        while(1) {
            LOG_DEBUG << "Waiting for next event to publish";
            size_t pending = 0;
            services::EventData e = m_events.pop(&pending);
            LOG_DEBUG << "Scheduling event. Pending " << pending;
            google::protobuf::Empty v;
            if (!client().call(&services::EventsBus::Stub::Notify, e, &v).ok()) {
                LOG_ERROR << "Can't publish update " << e.event();
            }
            else
            {
                LOG_DEBUG << "Event sent " << e.event();
            }
        }
    });
}

void EventsBaseImpl::subscribe(const std::string& sender, std::function<void(const std::shared_ptr<Event>)>&& handler)
{
    services::ConsumerIdentifier id;
    id.set_interface(Application::app()->identifier());
    id.set_instance(identifier());
    id.set_address(address());

    google::protobuf::Empty e;

    while(1) {
        if (client().call(&services::EventsBus::Stub::Subscribe, id, &e).ok()) {
            m_service.add(sender, handler);
            LOG_INFO << "Subscribed!";
            break;
        }
        else {
            LOG_ERROR << "Can't subscribe for events";
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

void EventsBaseImpl::notify(const std::shared_ptr<const Event> event)
{
    services::EventData e;
    e.set_sender(Application::app()->identifier());
    nlohmann::json msg = *event;
    e.set_event(msg.dump());

    m_events.push(e);
}

template<>
IEvents* Singleton<IEvents>::create()
{
    return new EventsBaseImpl();
}
