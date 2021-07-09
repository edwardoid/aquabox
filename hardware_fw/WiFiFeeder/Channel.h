#ifndef CHANNEL_H
#define CHANNEL_H

template<int Num>
struct Channel
{
    static class Times TimesProperty;
    static class aquabox::proto::ArduinoStreamIO HermesIO;
    static class HermesFeeder feeder;
    static aquabox::proto::byte_t* FEEDER_TOKEN;
    static aquabox::proto::byte_t FEEDER_SERIAL[SERIAL_LEN];
    Channel()
        : Feeder(TimesProperty, &HermesIO, FEEDER_TOKEN, FEEDER_SERIAL)
    {}
};

#endif // CHANNEL_H