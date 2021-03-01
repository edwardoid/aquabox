#pragma once

#include <vector>
#include "RF24.h"
#include "RF24Mesh.h"
#include "hermes_master.h"
#include <list>
#include <unordered_map>
#include "message.h"
#include "message_io.h"
#include <singleton.h>
#include <mutex>
#include <array>
#include <memory>
#include <s2s.h>

namespace aquabox
{
    class HermesMeshIO : public proto::MessageIO
    {
    public:
        HermesMeshIO(class HermesMeshGateway* gateway);
        virtual bool good() const override;
        virtual proto::buffer_length_t available() const override;
        virtual proto::buffer_length_t read(proto::buffer_length_t sz, proto::byte_t* buffer) override;
        virtual proto::buffer_length_t write(const proto::byte_t* buffer, proto::buffer_length_t sz) override;
        virtual proto::buffer_length_t wait(proto::buffer_length_t length) const;

        void newMessage(const RF24NetworkHeader& header, const proto::Message& msg);
        void clear();
    protected:
        friend class HermesMeshGateway;
        class HermesMeshGateway* m_gateway;
        uint16_t m_lastId;
        std::vector<proto::Message> m_messages;
    };

    class HermesMeshGateway
    {
    public:
        HermesMeshGateway();
        void lock() const;
        void unlock() const;

        struct serial_hasher
        {
            size_t operator()(const HermesSerial& serial) const {
                return *(reinterpret_cast<const uint64_t*>(serial.data()));
            }
        };

        SignalSlot::Signal<HermesSerial> reconnected;
        SignalSlot::Signal<std::shared_ptr<HermesMeshIO>, const HermesSerial&> newConnection;
        void remove(const HermesSerial& serial);
        void run();
    protected:
        bool isAlive(uint16_t from);
        bool write(uint16_t from, const proto::byte_t* buffer, proto::buffer_length_t sz);
        void handle(RF24NetworkHeader& header);
        inline RF24Network& network() { return m_network; }
        inline RF24Mesh& mesh() { return m_mesh; }
        void loop(bool processAll = true);
        bool waitForNext(std::chrono::seconds& timeout);
        void wait();

        friend class HermesMeshIO;
        
    private:
        bool m_stop = false;
        mutable std::mutex m_mx;
        RF24 m_radio;
        RF24Network m_network;
        RF24Mesh m_mesh;
        std::unordered_map<HermesSerial, std::shared_ptr<HermesMeshIO> , serial_hasher> m_clients;
    };
} // namespace aquabox