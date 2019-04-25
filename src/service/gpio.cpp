#include "gpio.h"
#include "log.h"
#include <string.h>
#include <map>

using namespace aquabox;

#ifndef TARGET_DEVICE
const std::string GPIO::SysClassGpio("./../../../arm/sysroot/sys/class/gpio");
const std::string GPIO::SysClassGpioExport("./../../../arm/sysroot/sys/class/gpio/export");
const std::string GPIO::SysClassGpioUnexport("./../../../arm/sysroot/sys/class/gpio/unexport");

#else
const std::string GPIO::SysClassGpio("/sys/class/gpio");
const std::string GPIO::SysClassGpioExport("/sys/class/gpio/export");
const std::string GPIO::SysClassGpioUnexport("/sys/class/gpio/unexport");
#endif 
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
    if (pos->second == nullptr)
        pos->second = std::make_shared<GPIO>(pos->first);

    return pos->second;
}

GPIO::GPIO(uint8_t pin)
    : Pin(pin)
    , ValueFile(SysClassGpio + std::string("/gpio") + std::to_string(pin) + std::string("/value"))
    , DirectionFile(SysClassGpio + std::string("/gpio") + std::to_string(pin) + std::string("/direction"))
    , m_mode(Mode::Unknown)
{
    Bytes b;
    send(b, 0);
}

GPIO::Mode GPIO::readMode()
{
    m_mode = Mode::Unknown;
    auto fp = fopen(DirectionFile.c_str(), "r");
    if (fp != nullptr) {
        char data[32];
        auto count = fread(data, sizeof(char), 32, fp);
        data[31] = '\0';
        fclose(fp);
        if (count < 2) {
            LOG_ERROR << "Read less than expected. Got " << count << " expected minimum 2";
            return m_mode;
        }

        if (strncmp(data, "in", 2) == 0)
            m_mode = Mode::In;
        else if (strncmp(data, "out", 3) == 0)
            m_mode = Mode::Out;
        else
            LOG_DEBUG << "Got " << data << " from " << DirectionFile;
    } else {
        LOG_DEBUG << "Can't open " << DirectionFile << " error: " << strerror(errno);
    }

    return m_mode;
}

bool GPIO::writeMode(Mode mode) {
    if (mode == Mode::Unknown)
        return close();

    auto fp = fopen(DirectionFile.c_str(), "w");
    if (fp != nullptr) {
        const char* dir = mode == Mode::In ? "in" : "out";

        auto count = fwrite(dir, sizeof(char), strlen(dir) + 1, fp);
        fclose(fp);
        
        m_mode = count == (strlen(dir) + 1) ? mode : Mode::Unknown;
    } else {
        LOG_DEBUG << "Can't open " << DirectionFile << " error: " << strerror(errno);
    }

    return m_mode == mode;
}

bool GPIO::exported() const
{
    struct stat buffer;   
    return (stat (DirectionFile.c_str(), &buffer) == 0) &&
           (stat (ValueFile.c_str(), &buffer) == 0); 
}

bool GPIO::open()
{
    if (exported()) {
        readMode();
        return true;
    }
    LOG_DEBUG << "Opening " << SysClassGpioExport << " for " << (int)Pin;
    auto fp = fopen(SysClassGpioExport.c_str(), "w");
    if (fp == nullptr) {
        LOG_ERROR << "Can't open " << SysClassGpioExport << " errno:" << strerror(errno);
        return false;
    }
    
    std::string num = std::to_string(Pin);
    auto count = fwrite(num.c_str(), sizeof(std::string::value_type), num.size() + 1, fp);
    fclose(fp);

    return count == num.size() + 1 && readMode() != Mode::Unknown;
}

bool GPIO::close()
{
    if (!exported())
        return true;


    LOG_DEBUG << "Opening " << SysClassGpioUnexport << " for " << (int)Pin;

    auto fp = fopen(SysClassGpioUnexport.c_str(), "w");
    if (fp == nullptr) {
        LOG_ERROR << "Can't open " << SysClassGpioUnexport << " error:" << strerror(errno);
        return false;
    }
    
    std::string num = std::to_string(Pin);
    auto count = fwrite(num.c_str(), sizeof(std::string::value_type), num.size() + 1, fp);
    fflush(fp);
    fclose(fp);

    m_mode = Mode::Unknown;
    return count == num.size() + 1;
}

bool GPIO::isHigh()
{
    if (!exported()) {
        return false;
    }
    auto fp = fopen(ValueFile.c_str(), "r");
    if (fp != nullptr) {
        char data[] = { 0, 0 };
        auto count = fread(data, sizeof(char), 2, fp);
        fclose(fp);
        return count > 0 ? data[0] == '1' : false;
    } else {
        LOG_DEBUG << "Can't open " << ValueFile << " error: " << strerror(errno);
    }
    return false;
}

BytesCount GPIO::send(const Bytes& data, const BytesCount count)
{
    if (m_mode == Mode::Unknown) {
        if (!open())
            return 0;
    }
    if (m_mode != Mode::Out) {
        if (!writeMode(Mode::Out))
            return 0;
    }

    auto fp = fopen(ValueFile.c_str(), "w");
    if (fp != nullptr) {
        if (count > 0)
        {
            const char v[] = { static_cast<char>(data[0] + '0'), 0 };
            auto count = fwrite(&v, sizeof(char), 2, fp);
            fflush(fp);
        }
        fclose(fp);
        return count > 0;
    } else {
        LOG_DEBUG << "Can't open " << DirectionFile << " error: " << strerror(errno);
    }

    return 0;
}

BytesCount GPIO::read(Bytes& data, const Offset)
{
    if (m_mode == Mode::Unknown) {
        if (!open())
            return 0;
    }
    if (m_mode != Mode::In) {
        if (!writeMode(Mode::In))
            return 0;
    }

    if (data.empty())
        return 0;
    data[0] = isHigh() ? 1 : 0;
    return 1;
}
