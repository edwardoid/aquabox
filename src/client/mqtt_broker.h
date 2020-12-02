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


#include <mqtt_server_cpp.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

namespace aquabox
{
    class MQTTBroker
    {
    private:
        using ServerImpl = MQTT_NS::server<>;
        using MQTTSlave = MQTT_NS::server<>::endpoint_t;
        using MQTTSlavePtr = std::shared_ptr<MQTTSlave>;
        struct MQTTSlaveSubscription {
            MQTTSlaveSubscription(MQTT_NS::buffer topic, MQTTSlavePtr con, MQTT_NS::qos qos_value);
            MQTT_NS::buffer topic;
            MQTTSlavePtr connection;
            MQTT_NS::qos qos_value;
        };

        struct tag_topic {};
        struct tag_con {};

        using MQTTSubscriptionsMap = boost::multi_index::multi_index_container<
                MQTTSlaveSubscription,
                boost::multi_index::indexed_by<
                    boost::multi_index::ordered_non_unique<
                        boost::multi_index::tag<tag_topic>,
                        BOOST_MULTI_INDEX_MEMBER(MQTTSlaveSubscription, MQTT_NS::buffer, topic)
                    >,
                    boost::multi_index::ordered_non_unique<
                        boost::multi_index::tag<tag_con>,
                        BOOST_MULTI_INDEX_MEMBER(MQTTSlaveSubscription, MQTTSlavePtr, connection)
                    >
                >
            >;
    public:
        MQTTBroker(){}
        void start(int port);
        void stop();

        bool authorize(const std::string& username, const std::string& password, const std::string& clientId) { return true; }
        void onNewMessage(MQTTSlavePtr sender, const std::string& topic, const std::vector<uint8_t>& data);
        void sendMessage(const std::string& topic, const std::vector<uint8_t>& data);

    private:
        void setupHandlers();
        void disconnectSlave(MQTTSlavePtr slave);

        void onAccept(MQTTSlavePtr slave);
        void onError(MQTT_NS::error_code code);
    private:
        boost::asio::io_context m_serviceContext;
        std::set<MQTTSlavePtr> m_connections;
        MQTTSubscriptionsMap m_subscriptions;
        ServerImpl* m_server = nullptr;
    };
}