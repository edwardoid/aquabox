#include "mqtt_bus.h"
#include "log.h"
#include <thread>
#include "settings.h"

using namespace aquabox;

MQTTBus::MQTTBus(const std::string& address)
   : Bus(address)
{
   static std::thread t([this]() {
      m_broker.start(Settings::integer(Settings::Keys::MQTT_PORT, Settings::Defaults::MQTT_PORT));
   });
}

MQTTBus::~MQTTBus()
{}

std::shared_ptr<Hardware> MQTTBus::hw(const std::string& hw) {
   return nullptr;
}