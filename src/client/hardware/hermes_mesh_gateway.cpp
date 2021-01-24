#include "hermes_mesh_gateway.h"
#include <log.h>
#include <thread>

using namespace aquabox;

HermesMeshIO::HermesMeshIO()
{}

bool HermesMeshIO::good() const
{
    std::lock_guard<HermesMeshGateway> lock(* Singleton<HermesMeshGateway>::i());
    return Singleton<HermesMeshGateway>::i()->mesh().getAddress(m_lastId) > 0;
}

proto::buffer_length_t HermesMeshIO::available() const
{
    return sizeof(MessageIO) * m_messages.size();
}

proto::buffer_length_t HermesMeshIO::read(proto::buffer_length_t sz, proto::byte_t* buffer)
{
    static const size_t portion = sizeof(MessageIO);
    sz /= portion;
    if (sz > 0)
    {
        size_t skip = m_messages.size() - sz;
        memcpy(buffer, m_messages.data() + skip * portion, sz * portion);
        m_messages.erase(m_messages.begin() + skip, m_messages.end());
    }

    return portion * sz;
}

proto::buffer_length_t HermesMeshIO::write(const proto::byte_t* buffer, proto::buffer_length_t sz)
{
    std::lock_guard<HermesMeshGateway> lock(* Singleton<HermesMeshGateway>::i());
    return Singleton<HermesMeshGateway>::i()->mesh().write(m_lastId, buffer, 'H', sz);
}

bool HermesMeshIO::makeRequest(const proto::Message& req, proto::Message& rsp)
{
    std::lock_guard<HermesMeshGateway> lock(* Singleton<HermesMeshGateway>::i());
    auto sz = m_messages.size();
    auto now = std::chrono::system_clock::now();
    if(!Singleton<HermesMeshGateway>::i()->mesh().write(m_lastId, &req, 'H', sizeof(req))) {
        return false;
    }
    while(sz <= m_messages.size() && (std::chrono::system_clock::now() - now) < std::chrono::seconds(3)) {
        Singleton<HermesMeshGateway>::i()->loop(false);
        if(sz < m_messages.size()) {
            break;
        }
        Singleton<HermesMeshGateway>::i()->wait();
    }
    if (sz < m_messages.size()) {
        rsp = *m_messages.rbegin();
        m_messages.pop_back();
        return true;
    }
    return false;
}

void HermesMeshIO::newMessage(const RF24NetworkHeader& header, const proto::Message& msg)
{
    m_lastId = header.from_node;
    m_messages.push_back(msg);
}

HermesMeshGateway::HermesMeshGateway()
    : m_radio(22, 0), m_network(m_radio), m_mesh(m_radio, m_network)
{

}

void HermesMeshGateway::loop(bool processAll)
{
    m_mesh.update();
    m_mesh.DHCP();

    while(m_network.available()) {
        RF24NetworkHeader hdr;
        m_network.peek(hdr);
        if(hdr.type == 'H') // Hermes message
        {
            handle(hdr);
        } else if (hdr.type == 'D') {
            char msg[100];
            memset(msg, '\0', 100);
            m_network.read(hdr, &msg, 100);
            LOG_DEBUG << "Got from node " << hdr.from_node << ": " << msg;
        } else if (hdr.type == 'M') {
            uint32_t dat = 0;
            m_network.read(hdr, &dat, sizeof(dat));
            LOG_DEBUG << "Got from node[tst] " << hdr.from_node << ": " << dat;
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
    m_mesh.begin();
    while(m_stop) {
        std::lock_guard<HermesMeshGateway> lock(*this);
        loop();   
    }
}

void HermesMeshGateway::handle(RF24NetworkHeader& header)
{
    proto::Message msg;
    auto rcv = m_network.read(header, &msg, sizeof(proto::Message));
    if(sizeof(msg) != rcv) {
        LOG_ERROR << "Got " << rcv << " bytes out of " << sizeof(msg) << " via RF24 net";
        return;
    }
    serial_t serial;
    std::shared_ptr<HermesMeshIO> io;
    memcpy(serial.data(), msg.serial, SERIAL_LEN);
    bool isNew = m_clients.find(serial) == m_clients.end();
    if (isNew) {
        m_clients[serial] = std::make_shared<HermesMeshIO>();
    }
    m_clients[serial]->newMessage(header, msg);
    if (isNew) {
        newConnection(m_clients[serial]);
    }
}

void HermesMeshGateway::lock() const
{
    m_mx.lock();
}

void HermesMeshGateway::unlock() const
{
    m_mx.unlock();
}