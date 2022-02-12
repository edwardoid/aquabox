#include "hermes_mesh_gateway.h"
#include <common/log.h>
#include <common/timeout.h>
#include <micro_clients/settings.h>
#include <thread>

#include <iostream>

using namespace aquabox;

HermesMeshIO::HermesMeshIO(HermesMeshGateway* gateway)
    : m_gateway(gateway)
{}

bool HermesMeshIO::good() const
{
    return m_gateway->isAlive(m_lastId);
}

proto::buffer_length_t HermesMeshIO::available() const
{
    return sizeof(proto::Message) * m_messages.size();
}

proto::buffer_length_t HermesMeshIO::read(proto::buffer_length_t sz, proto::byte_t* buffer)
{
    static const size_t portion = sizeof(proto::Message);
    sz /= portion;
    if (sz > 0) {
        size_t skip = m_messages.size() - sz;
        auto pos = m_messages.begin() + skip;
        memcpy(buffer, &(*pos), sz * portion);
        m_messages.erase(pos, pos + sz);
    }

    return portion * sz;
}

proto::buffer_length_t HermesMeshIO::write(const proto::byte_t* buffer, proto::buffer_length_t sz)
{
    if (m_gateway->write(m_lastId, buffer, sz)) {
        return sz;
    }
    LOG_DEBUG << "Write failed!";
    return 0;
}

proto::buffer_length_t HermesMeshIO::wait(proto::buffer_length_t length) const
{
    LOG_DEBUG << "Waiting for " << length << " bytes from " << m_lastId;
    std::lock_guard<HermesMeshGateway> lock(*Singleton<HermesMeshGateway>::i());

    Timeout readTimeout(std::chrono::seconds(1000));
    while (available() < length && 0 == readTimeout.expired()) {
        m_gateway->loop(true);
        delay(20);
    }

    if (readTimeout.expired()) {
        LOG_WARNING << "Data waiting on " << m_lastId << " timed out";
    }

    return available();
}

void HermesMeshIO::newMessage(const RF24NetworkHeader& header, const proto::Message& msg)
{
    m_lastId = header.from_node;
    m_messages.push_back(msg);
}

void HermesMeshIO::clear()
{
    m_messages.clear();
}

HermesMeshGateway::HermesMeshGateway()
    : m_radio(Singleton<ISettings>::i()->get(Settings::Keys::RF24_RADIO_PIN, Settings::Defaults::RF24_RADIO_PIN),
              Singleton<ISettings>::i()->get(Settings::Keys::RF24_BUS, Settings::Defaults::RF24_BUS))
    , m_network(m_radio)
    , m_mesh(m_radio, m_network)
{}

bool HermesMeshGateway::isAlive(uint16_t from)
{
    std::lock_guard<HermesMeshGateway> lock(*Singleton<HermesMeshGateway>::i());
    return m_mesh.getNodeID(from) > 0;
}

void HermesMeshGateway::remove(const HermesSerial& serial)
{
    std::lock_guard<HermesMeshGateway> lock(*Singleton<HermesMeshGateway>::i());
    m_clients.erase(serial);
}

bool HermesMeshGateway::write(uint16_t from, const proto::byte_t* buffer, proto::buffer_length_t sz)
{
    std::lock_guard<HermesMeshGateway> lock(*Singleton<HermesMeshGateway>::i());
    auto now = std::chrono::system_clock::now();
    while (!m_mesh.write(from, buffer, 'M', sz)) {
        m_mesh.update();
        m_mesh.DHCP();
        if (std::chrono::system_clock::now() - now < std::chrono::seconds(3)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        else {
            return false;
        }
    }

    return true;
}

void HermesMeshGateway::loop(bool processAll)
{
    m_mesh.update();
    m_mesh.DHCP();

    while (m_network.available()) {
        RF24NetworkHeader hdr;
        m_network.peek(hdr);
        if (hdr.type == 'M') // Hermes message
        {
            handle(hdr);
        }
        else if (hdr.type == 'D') {
            char msg[100];
            memset(msg, '\0', 100);
            m_network.read(hdr, &msg, 100);
            LOG_DEBUG << "Got from node " << hdr.from_node << ": " << msg;
        }
        else if (hdr.type == 'P') {
            proto::Message p;
            m_network.read(hdr, &p, sizeof(proto::Message));
            p.payloadLength++;
            auto now = std::chrono::system_clock::now();
            while (!m_mesh.write(hdr.from_node, &p, 'P', sizeof(proto::Message))) {
                m_mesh.update();
                m_mesh.DHCP();
                delay(100);
                if (std::chrono::system_clock::now() - now > std::chrono::seconds(3)) {
                    break;
                }
            }
        }
        else {
            LOG_DEBUG << "Got a message!! Type: " << hdr.type;
        }

        if (!processAll) {
            return;
        }
    }
}

void HermesMeshGateway::wait()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
}

void HermesMeshGateway::run()
{
    m_mesh.setNodeID(0);
    if (!m_mesh.begin()) {
        LOG_ERROR << "Can't start mesh network!";
    }
    else {
        LOG_INFO << "Mesh network started";
    }

    while (!m_stop) {
        {
            std::lock_guard<HermesMeshGateway> lock(*this);
            loop();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void HermesMeshGateway::handle(RF24NetworkHeader& header)
{
    proto::Message msg;
    std::uint8_t* p = reinterpret_cast<std::uint8_t*>(&msg);
    auto remain = sizeof(msg);
    while (remain > 0) {
        auto got = m_network.read(header, p, remain);
        p += got;
        remain -= got;
    }
    HermesSerial serial(msg.serial);

    static const char* types_str[] = {"Error", "InternalError", "Command", "Handshake"};

    LOG_DEBUG << "Got messafe from " << serial.to_string() << " type " << types_str[(int) msg.type];
    bool isHandshake = msg.type == proto::MessageType::Handshake;
    bool isReconnect = isHandshake && m_clients.find(serial) != m_clients.end();
    if (m_clients.find(serial) == m_clients.end()) { // isHandshake || isReconnect) {
        if (isReconnect) {
            LOG_INFO << "Device " << serial.to_string() << " reconnected";
        }
        else {
            LOG_INFO << "Got new device " << serial.to_string();
        }
        m_clients[serial] = std::make_shared<HermesMeshIO>(this);
    }
    if (isReconnect) {
        m_clients[serial]->clear();
        reconnected(serial);
    }
    else if (isHandshake) {
        newConnection(m_clients[serial], serial);
    }
    m_clients[serial]->newMessage(header, msg);
}

void HermesMeshGateway::lock() const
{
    m_mx.lock();
}

void HermesMeshGateway::unlock() const
{
    m_mx.unlock();
}