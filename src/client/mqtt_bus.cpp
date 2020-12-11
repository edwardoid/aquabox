#include "mqtt_bus.h"
#include "log.h"
#include <thread>
#include "settings.h"

using namespace aquabox;

Mqtt::Mqtt()
{
   static std::thread t([this]() {
      m_broker.start(Settings::integer(Settings::Keys::MQTT_PORT, Settings::Defaults::MQTT_PORT));
   });

   t.detach();
}

std::shared_ptr<Hardware> Mqtt::hw(const std::string& hw) {
   return nullptr;
}