#ifndef HERMES_FEEDER_H
#define HERMES_FEEDER_H

#include <AquaboxProto.h>
#include "TimesProperty.h"

class HermesFeeder
{
    struct Slave: public aquabox::proto::EasySlave<6>
    {
      friend class HermesFeeder;

      inline Slave(SlavePropertyPtr *props, aquabox::proto::IO *io, const aquabox::proto::byte_t* serial, const aquabox::proto::byte_t* token)
        : aquabox::proto::EasySlave<6>(props, io, serial, token) {  setProps(props); }
      virtual bool processNextMessage() override
      {
          bool getResult = false;
          aquabox::proto::Message rcv;
          getResult = m_io->get(rcv);
          //yield();
          aquabox::proto::Message rpl;
          if (dispatch(&rcv, &rpl))
          {
              yield();
              m_io->send(rpl);
              yield();
          }
          return getResult;
      }
    };
    
    aquabox::proto::CachedSlaveProperty<uint32_t> TimesTimeoutProperty;
    aquabox::proto::CachedSlaveProperty<char*> ClassProperty;
    aquabox::proto::CachedSlaveProperty<char*> TypeProperty;
    aquabox::proto::CachedSlaveProperty<bool> CacheProperty;
    aquabox::proto::CachedSlaveProperty<int32_t> PingTimeoutProperty;
    Slave::SlavePropertyPtr Props[6];
    Slave Impl;
    
public:
    HermesFeeder(Times& TimesProperty, aquabox::proto::IO* io, aquabox::proto::byte_t* Token, aquabox::proto::byte_t* Serial)
      : TimesTimeoutProperty("_times", 1000)
      , ClassProperty("class", "step")
      , TypeProperty("type", "feeder")
      , CacheProperty("_cache", false)
      , PingTimeoutProperty("_timeout", 5000)
      , Props({ &TimesProperty, &TimesTimeoutProperty, &ClassProperty, &TypeProperty, &CacheProperty, &PingTimeoutProperty })
      , Impl(Props, io, Serial, Token)
    {}

    inline bool handshake() { return Impl.handshake(); }
    inline bool processNextMessage() { return Impl.processNextMessage(); }
};

//aquabox::proto::byte_t* HermesFeeder::FEEDER_TOKEN = (aquabox::proto::byte_t*)("allow me");
//aquabox::proto::byte_t HermesFeeder::FEEDER_SERIAL[] PROGMEM = { 't', 's', 't', 'f', 'e', 'e', 'd', 'r' };

//aquabox::proto::ArduinoStreamIO HermesIO = aquabox::proto::ArduinoStreamIO();
//HermesFeeder Feeder(TimesProperty, &HermesIO);

#endif // HERMES_FEEDER_H
