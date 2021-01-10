#include "mqtt_broker.h"
#include <log.h>
#include "base64.h"

using namespace aquabox;

MQTTBroker::MQTTSlaveSubscription::MQTTSlaveSubscription(MQTT_NS::buffer topic, MQTTSlavePtr con, MQTT_NS::qos qos_value)
    :topic(std::move(topic)), connection(std::move(con)), qos_value(qos_value)
{}

void MQTTBroker::start(int port)
{
    if (m_server != nullptr) {
        stop();
    }
    m_server = new ServerImpl(
        boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port),
        m_serviceContext
    );

    setupHandlers();

    m_server->listen();

    m_serviceContext.run();
}

void MQTTBroker::stop()
{
    m_serviceContext.stop();
    if (m_server != nullptr) {
        delete m_server;
    }

    m_subscriptions.clear();
    m_connections.clear();
}

void MQTTBroker::setupHandlers()
{
    m_server->set_error_handler([this](MQTT_NS::error_code code) { onError(code); });
    m_server->set_accept_handler([this](MQTTSlavePtr slave) { onAccept(slave); });
}

void MQTTBroker::onError(MQTT_NS::error_code c)
{
    LOG_ERROR << c.message();
}

void MQTTBroker::onAccept(MQTTSlavePtr slavePtr)
{
    auto& slave = *slavePtr;
    std::weak_ptr<MQTTSlave> slaveRef(slavePtr);

    using packet_id_t = typename std::remove_reference_t<decltype(slave)>::packet_id_t;
    LOG_INFO << "New connection via MQTT";

    // Pass spep to keep lifetime.
    // It makes sure wp.lock() never return nullptr in the handlers below
    // including close_handler and error_handler.
    slave.start_session(std::move(slavePtr));

    // set connection (lower than MQTT) level handlers
    slave.set_close_handler(
        [this, slaveRef]
        (){ LOG_INFO << "MQTT connection to slave closed."; disconnectSlave(slaveRef.lock()); });
    slave.set_error_handler(
        [this, slaveRef]
        (MQTT_NS::error_code ec)
        { LOG_ERROR << "Error occured " << ec.message(); disconnectSlave(slaveRef.lock()); });

    // set MQTT level handlers
    slave.set_connect_handler(
        [this, slaveRef]
        (MQTT_NS::buffer client_id,
            MQTT_NS::optional<MQTT_NS::buffer> username,
            MQTT_NS::optional<MQTT_NS::buffer> password,
            MQTT_NS::optional<MQTT_NS::will>,
            bool clean_session,
            std::uint16_t keep_alive) {
            using namespace MQTT_NS::literals;
            // std::cout << "[server] client_id    : " << client_id << std::endl;
            // std::cout << "[server] username     : " << (username ? username.value() : "none"_mb) << std::endl;
            // std::cout << "[server] password     : " << (password ? password.value() : "none"_mb) << std::endl;
            // std::cout << "[server] clean_session: " << std::boolalpha << clean_session << std::endl;
            // std::cout << "[server] keep_alive   : " << keep_alive << std::endl;

            auto sp = slaveRef.lock();
            if (authorize(username ? username.value().to_string() : "",
                          password ? password.value().to_string() : "",
                          client_id.to_string())) {
                this->m_connections.insert(sp);
                sp->connack(false, MQTT_NS::connect_return_code::accepted);
            } else {
                sp->connack(false, MQTT_NS::connect_return_code::not_authorized);
            }
            return true;
        }
    );
    slave.set_disconnect_handler([this, slaveRef] () { std::cout << "[server] disconnect received." << std::endl; disconnectSlave(slaveRef.lock()); });
    slave.set_puback_handler([] (packet_id_t packet_id) { std::cout << "[server] puback received. packet_id: " << packet_id << std::endl; return true; });
    slave.set_pubrec_handler([] (packet_id_t packet_id) { std::cout << "[server] pubrec received. packet_id: " << packet_id << std::endl; return true; });
    slave.set_pubrel_handler([] (packet_id_t packet_id) { std::cout << "[server] pubrel received. packet_id: " << packet_id << std::endl; return true; });
    slave.set_pubcomp_handler([] (packet_id_t packet_id){ std::cout << "[server] pubcomp received. packet_id: " << packet_id << std::endl; return true; });
    slave.set_publish_handler(
        [this, slaveRef]
        (MQTT_NS::optional<packet_id_t> packet_id,
            MQTT_NS::publish_options pubopts,
            MQTT_NS::buffer topic_name,
            MQTT_NS::buffer contents){
            // std::cout << "[server] publish received."
            //             << " dup: "    << pubopts.get_dup()
            //             << " qos: "    << pubopts.get_qos()
            //             << " retain: " << pubopts.get_retain() << std::endl;
            // if (packet_id)
            //     std::cout << "[server] packet_id: " << *packet_id << std::endl;
            // std::cout << "[server] topic_name: " << topic_name << std::endl;
            // std::cout << "[server] contents: " << contents << std::endl;

            std::string encodedData = contents.to_string();
            
            std::vector<uint8_t> bytes = aquabox::utils::base64_decode<std::string, std::vector<uint8_t>>(encodedData, encodedData.size());

            onNewMessage(slaveRef.lock(), topic_name.to_string(), bytes);
            
            return true;
        });
    slave.set_subscribe_handler(
        [this, slaveRef]
        (packet_id_t packet_id,
            std::vector<std::tuple<MQTT_NS::buffer, MQTT_NS::subscribe_options>> entries) {
            std::vector<MQTT_NS::suback_return_code> res;
            res.reserve(entries.size());
            auto sp = slaveRef.lock();
            for (auto const& e : entries) {
                MQTT_NS::buffer topic = std::get<0>(e);
                MQTT_NS::qos qos_value = std::get<1>(e).get_qos();
                LOG_INFO << "Subscribed to topic: " << topic  << " qos: " << qos_value;
                res.emplace_back(MQTT_NS::qos_to_suback_return_code(qos_value));
                m_subscriptions.emplace(std::move(topic), sp, qos_value);
            }
            sp->suback(packet_id, res);
            return true;
        }
    );
    slave.set_unsubscribe_handler(
        [this, slaveRef]
        (packet_id_t packet_id,
            std::vector<MQTT_NS::buffer> topics) {
            for (auto const& topic : topics) {
                LOG_INFO << "Unsubscribing from " << topic;
                m_subscriptions.erase(topic);
            }
            slaveRef.lock()->unsuback(packet_id);
            return true;
        }
    );
}

void MQTTBroker::disconnectSlave(MQTTSlavePtr slave)
{
    m_connections.erase(slave);

    auto& idx = m_subscriptions.get<tag_con>();
    auto r = idx.equal_range(slave);
    idx.erase(r.first, r.second);
}

void MQTTBroker::onNewMessage(MQTTSlavePtr sender, const std::string& topic, const std::vector<uint8_t>& data)
{
    
}

void MQTTBroker::sendMessage(const std::string& topic, const std::vector<uint8_t>& data)
{
    auto const& idx = m_subscriptions.get<tag_topic>();
    std::string b64 = aquabox::utils::base64_encode<std::string>(data.data(), data.size());
    auto r = idx.equal_range(MQTT_NS::allocate_buffer(topic.cbegin(), topic.cend()));
    for (; r.first != r.second; ++r.first) {
        r.first->connection->publish(
            topic,
            b64,
            r.first->qos_value
        );
    }
}
