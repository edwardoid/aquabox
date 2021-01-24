#pragma once

#include <vector>
#include "RF24.h"
#include "RF24Mesh.h"
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
        HermesMeshIO();
        virtual bool good() const override;
        virtual proto::buffer_length_t available() const override;
        virtual bool makeRequest(const proto::Message& req, proto::Message& rsp) override;
        virtual proto::buffer_length_t read(proto::buffer_length_t sz, proto::byte_t* buffer) override;
        virtual proto::buffer_length_t write(const proto::byte_t* buffer, proto::buffer_length_t sz) override;

        void newMessage(const RF24NetworkHeader& header, const proto::Message& msg);
    private:
        uint16_t m_lastId;
        std::vector<proto::Message> m_messages;
    };

    class HermesMeshGateway
    {
    public:
        HermesMeshGateway();
        void lock() const;
        void unlock() const;

        using serial_t = std::array<uint8_t, SERIAL_LEN>;
        struct serial_hasher
        {
            size_t operator()(const serial_t& serial) const {
                return *(reinterpret_cast<const uint64_t*>(serial.data()));
            }
        };

        SignalSlot::Signal<std::shared_ptr<HermesMeshIO>> newConnection;
        void run();
    protected:
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
        std::unordered_map<serial_t, std::shared_ptr<HermesMeshIO> , serial_hasher> m_clients;
    };
} // namespace aquabox