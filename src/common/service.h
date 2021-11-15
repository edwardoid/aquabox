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

#pragma once
#include <iostream>
#include <map>
#include <vector>

namespace aquabox {

class Services {
protected:
    friend class AbstractService;
    static uint32_t implements(int Id, class AbstractService* impl)
    {
        static uint32_t m_id = 0;
        m_services[Id][++m_id] = impl;
        return m_id;
    }

    static void unregister(int Id, uint32_t instance)
    {
        m_services[Id].erase(instance);
        if (m_services[Id].empty()) {
            m_services.erase(Id);
        }
    }

public:
    template <typename Interface>
    static std::vector<Interface*> implementations();

    template <typename Interface, typename Function, typename... Args>
    static void invoke(Function, Args... args);

private:
    static std::map<uint32_t, std::map<uint32_t, class AbstractService*>> m_services;
};

class AbstractService {
protected:
    virtual ~AbstractService() { }
    uint32_t registerImplmentation(int Id)
    {
        return Services::implements(Id, this);
    }

    void unregisterImplementation(int Id, uint32_t instance)
    {
        Services::unregister(Id, instance);
    }
};

template <class Interface>
class Service : public AbstractService {
protected:
    Service()
    {
        m_implementationId = AbstractService::registerImplmentation(Id);
    }

    ~Service()
    {
        AbstractService::unregisterImplementation(Id, m_implementationId);
    }

    virtual bool initialize() { return true; }
    virtual bool initialized() const { return true; }

protected:
    static const uint32_t Id;
    friend class Services;

private:
    int32_t m_implementationId;
};

template <typename Interface>
std::vector<Interface*> Services::implementations()
{
    std::vector<Interface*> impls;
    for (auto i : m_services[Service<Interface>::Id]) {
        Interface* impl = dynamic_cast<Interface*>(i.second);
        if (impl) {
            impls.push_back(impl);
        }
    }

    return impls;
}

template <typename Interface, typename Function, typename... Args>
void Services::invoke(Function fn, Args... args)
{
    for (auto i : implementations<Interface>()) {
        (i->*fn)(args...);
    }
}

/*
    struct Ev {};



    struct SayHello: public Service<class SayHello>
    {
    public:
        virtual void hello() = 0;
    };

    struct EventConsumer: public Service<class EventConsumer>
    {
        virtual void happened(const Ev& e) = 0;
    };

    struct EventProducer: public Service<class EventProducer>
    {
    public:
        virtual void emit(const Ev& e) = 0;
    };

    template<>
    const uint32_t Service<EventProducer>::Id = 0;

    template<>
    const uint32_t Service<EventConsumer>::Id = 1;

    template<>
    const uint32_t Service<SayHello>::Id = 2;

    class DeviceManager: public EventConsumer, public EventProducer, public SayHello
    {
    public:
        DeviceManager() {}

        virtual void happened(const Ev& e) override {};
        virtual void emit(const Ev& e) override {};
        virtual void hello() override {
            std::cout << "Hello!" << std::endl;
        };
    };


    struct DeviceManagerEx: public SayHello
    {
        virtual void hello() override {
            std::cout << "Bye!" << std::endl;
        };
    };

    struct Complex: virtual public DeviceManager, virtual public DeviceManagerEx
    {

    };*/
}

/*
int main()
{
    aquabox::DeviceManagerEx dm1;
    std::cout << "Consumers " << aquabox::Services::implementations<aquabox::SayHello>().size() << std::endl;
    aquabox::Services::invoke<aquabox::SayHello>(&aquabox::SayHello::hello);
    return 0;
}*/