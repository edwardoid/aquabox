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
#include "service.h"

namespace aquabox
{
    std::map<uint32_t, std::map<uint32_t, class AbstractService*>> Services::m_services;
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