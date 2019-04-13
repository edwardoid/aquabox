#include "gpio.h"
#include <string.h>
#include <map>

using namespace aquabox;

const std::string GPIO::SysClassGpio("/sys/gpio");
const std::string GPIO::SysClassGpioExport("/sys/gpio/export");
const std::string GPIO::SysClassGpioUnexport("/sys/gpio/unexport");

std::shared_ptr<GPIO> GPIO::get(uint8_t pin)
{
    static std::map<uint8_t, std::shared_ptr<GPIO>> pins({
        { 12, nullptr },
        { 11, nullptr },
        { 6, nullptr },
        { 1, nullptr },
        { 0, nullptr },
        { 3, nullptr },
        { 15, nullptr },
        { 16, nullptr },
        { 14, nullptr },
        { 198, nullptr },
        { 199, nullptr },
        { 7, nullptr },
        { 19, nullptr },
        { 18, nullptr },
        { 2, nullptr },
        { 13, nullptr },
        { 10, nullptr }
    });

    auto pos = pins.find(pin);
    if (pos == pins.end())
        return nullptr;
    pos->second = std::make_shared<GPIO>(pos->first);

    return pos->second;
}

GPIO::GPIO(uint8_t pin)
    : Pin(pin)
    , ValueFile(SysClassGpio + std::string("/gpio") + std::to_string(pin) + std::string("/value"))
    , DirectionFile(SysClassGpio + std::string("/gpio") + std::to_string(pin) + std::string("/direction"))
    , m_mode(Mode::Unknown)
{}

GPIO::Mode GPIO::readMode()
{
    m_mode = Mode::Unknown;
    auto fp = fopen(DirectionFile.c_str(), "r");
    if (fp != nullptr) {
        char data[32];
        auto count = fread(data, sizeof(char), 32, fp);
        data[31] = '\0';
        fclose(fp);
        if (count < 2)
            return m_mode;

        if (strlen(data) != count)
            return m_mode;

        if (strcmp(data, "in") == 0)
            m_mode = Mode::In;
        else if (strcmp(data, "out") == 0)
            m_mode = Mode::Out;
    }

    return m_mode;
}

bool GPIO::writeMode(Mode mode) {
    if (mode == Mode::Unknown)
        return close();

    auto fp = fopen(DirectionFile.c_str(), "w");
    if (fp != nullptr) {
        const char* dir = mode == Mode::In ? "in" : "out";

        auto count = fwrite(dir, sizeof(char), strlen(dir), fp);
        fclose(fp);
        
        m_mode = count == strlen(dir) ? mode : Mode::Unknown;
    }

    return m_mode == mode;
}

bool GPIO::open()
{
    if (readMode() != Mode::Unknown)
        close();
    auto fp = fopen(SysClassGpioExport.c_str(), "w");
    if (fp == nullptr)
        return false;
    
    std::string num = std::to_string(Pin);
    auto count = fwrite(num.c_str(), sizeof(std::string::value_type), num.size(), fp);
    fclose(fp);

    return count == num.size() && readMode() != Mode::Unknown;
}

bool GPIO::close()
{
    if (readMode() == Mode::Unknown)
        return true;

    auto fp = fopen(SysClassGpioUnexport.c_str(), "w");
    if (fp == nullptr)
        return false;
    
    std::string num = std::to_string(Pin);
    auto count = fwrite(num.c_str(), sizeof(std::string::value_type), num.size(), fp);
    fclose(fp);

    readMode();
    return count == num.size();
}

BytesCount GPIO::send(const Bytes& data, const BytesCount count)
{
    if (m_mode == Mode::Unknown) {
        if (!open())
            return 0;
    }
    if (m_mode == Mode::In) {
        if (!writeMode(Mode::Out))
            return 0;
    }

    auto fp = fopen(DirectionFile.c_str(), "w");
    if (fp != nullptr && count > 0) {

        char v = data[0];
        v += '0';
        auto count = fwrite(&v, sizeof(char), 1, fp);
        fclose(fp);
        
        return count > 0;
    }

    return 0;
}

BytesCount GPIO::read(Bytes& data, const Offset)
{
    if (m_mode == Mode::Unknown) {
        if (!open())
            return 0;
    }
    if (m_mode == Mode::Out) {
        if (!writeMode(Mode::In))
            return 0;
    }

    auto fp = fopen(ValueFile.c_str(), "r");
    if (fp != nullptr) {
        auto count = fread(data.data(), sizeof(Bytes::value_type), data.size(), fp);
        fclose(fp);
        return count;
    }
    return 0;
}
