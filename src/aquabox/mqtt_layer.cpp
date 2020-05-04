#include "mqtt_layer.h"
#include "log.h"

#include <set>

#include <mqtt_server_cpp.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>


namespace mi = boost::multi_index;

using con_t = MQTT_NS::server<>::endpoint_t;
using con_sp_t = std::shared_ptr<con_t>;

struct sub_con {
    sub_con(MQTT_NS::buffer topic, con_sp_t con, MQTT_NS::qos qos_value)
        :topic(std::move(topic)), con(std::move(con)), qos_value(qos_value) {}
    MQTT_NS::buffer topic;
    con_sp_t con;
    MQTT_NS::qos qos_value;
};

struct tag_topic {};
struct tag_con {};

using mi_sub_con = mi::multi_index_container<
    sub_con,
    mi::indexed_by<
        mi::ordered_non_unique<
            mi::tag<tag_topic>,
            BOOST_MULTI_INDEX_MEMBER(sub_con, MQTT_NS::buffer, topic)
        >,
        mi::ordered_non_unique<
            mi::tag<tag_con>,
            BOOST_MULTI_INDEX_MEMBER(sub_con, con_sp_t, con)
        >
    >
>;


inline void close_proc(std::set<con_sp_t>& cons, mi_sub_con& subs, con_sp_t const& con) {
    cons.erase(con);

    auto& idx = subs.get<tag_con>();
    auto r = idx.equal_range(con);
    idx.erase(r.first, r.second);
}

using namespace aquabox;

std::shared_ptr<Bus> Mqtt::bus(std::string bus)
{
    boost::asio::io_context ioc;

    auto s = MQTT_NS::server<>(
        boost::asio::ip::tcp::endpoint(
            boost::asio::ip::tcp::v4(),
            boost::lexical_cast<std::uint16_t>(1888)
        ),
        ioc
    );

    s.set_error_handler(
        [](MQTT_NS::error_code ec) {
            LOG_ERROR << ec.message();
        }
    );

    std::set<con_sp_t> connections;
    mi_sub_con subs;

    s.set_accept_handler(
        [&connections, &subs](con_sp_t spep) {
            auto& ep = *spep;
            std::weak_ptr<con_t> wp(spep);

            using packet_id_t = typename std::remove_reference_t<decltype(ep)>::packet_id_t;
            LOG_INFO << "accept";

            // Pass spep to keep lifetime.
            // It makes sure wp.lock() never return nullptr in the handlers below
            // including close_handler and error_handler.
            ep.start_session(std::move(spep));

            // set connection (lower than MQTT) level handlers
            ep.set_close_handler(
                [&connections, &subs, wp]
                (){
                    LOG_INFO << "[server] closed.";
                    auto sp = wp.lock();
                    BOOST_ASSERT(sp);
                    close_proc(connections, subs, sp);
                });
            ep.set_error_handler(
                [&connections, &subs, wp]
                (MQTT_NS::error_code ec){
                    LOG_INFO << "[server] error: " << ec.message();
                    auto sp = wp.lock();
                    BOOST_ASSERT(sp);
                    close_proc(connections, subs, sp);
                });

            // set MQTT level handlers
            ep.set_connect_handler(
                [&connections, wp]
                (MQTT_NS::buffer client_id,
                 MQTT_NS::optional<MQTT_NS::buffer> username,
                 MQTT_NS::optional<MQTT_NS::buffer> password,
                 MQTT_NS::optional<MQTT_NS::will>,
                 bool clean_session,
                 std::uint16_t keep_alive) {
                    using namespace MQTT_NS::literals;
                    LOG_INFO << "[server] client_id    : " << client_id;
                    LOG_INFO << "[server] username     : " << (username ? username.value() : "none"_mb);
                    LOG_INFO << "[server] password     : " << (password ? password.value() : "none"_mb);
                    LOG_INFO << "[server] clean_session: " << std::boolalpha << clean_session;
                    LOG_INFO << "[server] keep_alive   : " << keep_alive;
                    auto sp = wp.lock();
                    BOOST_ASSERT(sp);
                    connections.insert(sp);
                    sp->connack(false, MQTT_NS::connect_return_code::accepted);
                    return true;
                }
            );
            ep.set_disconnect_handler(
                [&connections, &subs, wp]
                (){
                    LOG_INFO << "[server] disconnect received.";
                    auto sp = wp.lock();
                    BOOST_ASSERT(sp);
                    close_proc(connections, subs, sp);
                });
            ep.set_puback_handler(
                []
                (packet_id_t packet_id){
                    LOG_INFO << "[server] puback received. packet_id: " << packet_id;
                    return true;
                });
            ep.set_pubrec_handler(
                []
                (packet_id_t packet_id){
                    LOG_INFO << "[server] pubrec received. packet_id: " << packet_id;
                    return true;
                });
            ep.set_pubrel_handler(
                []
                (packet_id_t packet_id){
                    LOG_INFO << "[server] pubrel received. packet_id: " << packet_id;
                    return true;
                });
            ep.set_pubcomp_handler(
                []
                (packet_id_t packet_id){
                    LOG_INFO << "[server] pubcomp received. packet_id: " << packet_id;
                    return true;
                });
            ep.set_publish_handler(
                [&subs]
                (MQTT_NS::optional<packet_id_t> packet_id,
                 MQTT_NS::publish_options pubopts,
                 MQTT_NS::buffer topic_name,
                 MQTT_NS::buffer contents){
                    LOG_INFO << "[server] publish received."
                              << " dup: "    << pubopts.get_dup()
                              << " qos: "    << pubopts.get_qos()
                              << " retain: " << pubopts.get_retain();
                    if (packet_id)
                        LOG_INFO << "[server] packet_id: " << *packet_id;
                    LOG_INFO << "[server] topic_name: " << topic_name;
                    LOG_INFO << "[server] contents: " << contents;
                    auto const& idx = subs.get<tag_topic>();
                    auto r = idx.equal_range(topic_name);
                    for (; r.first != r.second; ++r.first) {
                        r.first->con->publish(
                            topic_name,
                            contents,
                            std::min(r.first->qos_value, pubopts.get_qos())
                        );
                    }
                    return true;
                });
            ep.set_subscribe_handler(
                [&subs, wp]
                (packet_id_t packet_id,
                 std::vector<std::tuple<MQTT_NS::buffer, MQTT_NS::subscribe_options>> entries) {
                    LOG_INFO << "[server]subscribe received. packet_id: " << packet_id;
                    std::vector<MQTT_NS::suback_return_code> res;
                    res.reserve(entries.size());
                    auto sp = wp.lock();
                    BOOST_ASSERT(sp);
                    for (auto const& e : entries) {
                        MQTT_NS::buffer topic = std::get<0>(e);
                        MQTT_NS::qos qos_value = std::get<1>(e).get_qos();
                        LOG_INFO << "[server] topic: " << topic  << " qos: " << qos_value;
                        res.emplace_back(MQTT_NS::qos_to_suback_return_code(qos_value));
                        subs.emplace(std::move(topic), sp, qos_value);
                    }
                    sp->suback(packet_id, res);
                    return true;
                }
            );
            ep.set_unsubscribe_handler(
                [&subs, wp]
                (packet_id_t packet_id,
                 std::vector<MQTT_NS::buffer> topics) {
                    LOG_INFO << "[server]unsubscribe received. packet_id: " << packet_id;
                    for (auto const& topic : topics) {
                        subs.erase(topic);
                    }
                    auto sp = wp.lock();
                    BOOST_ASSERT(sp);
                    sp->unsuback(packet_id);
                    return true;
                }
            );
        }
    );

    s.listen();

    ioc.run();

    return nullptr;
}