#include "mqtt_bus.h"
#include "../mqtt/mqtt_broker.h"
#include <common/log.h>
#include <common/singleton.h>
#include <micro_clients/settings.h>
#include <thread>

using namespace aquabox;

MQTTBus::MQTTBus(const std::string& address)
    : Bus(address)
    , m_broker(std::make_unique<MQTTBroker>())
{
    static std::thread t([this]() {
        try{
            m_broker->start(Singleton<ISettings>::i()->port(Settings::Keys::MQTT_PORT, Settings::Defaults::MQTT_PORT));
        } catch(boost::wrapexcept<boost::system::system_error> e) {
            LOG_ERROR << e.what();
        }
    });
}

MQTTBus::~MQTTBus() { }

std::shared_ptr<Hardware> MQTTBus::hw(const std::string& hw) { return nullptr; }