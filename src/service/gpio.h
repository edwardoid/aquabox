#ifndef GPIO_H
#define GPIO_H

#include "protocol.h"
#include <memory>
#include <string>

namespace aquabox
{
    class GPIO: public Protocol
    {
    public:
        GPIO(uint8_t pin);
        enum class Mode
        {
            Unknown,
            In,
            Out
        };
    private:
        Mode readMode();
        bool writeMode(Mode mode);
    public:
        static std::shared_ptr<GPIO> get(uint8_t pin);
        bool open() override;
        bool close() override;
        BytesCount send(const Bytes& data, const BytesCount count = 0) override;
        BytesCount read(Bytes& data, const Offset = 0) override;
    private:
        static const std::string SysClassGpio;
        static const std::string SysClassGpioExport;
        static const std::string SysClassGpioUnexport;
        uint8_t Pin;
        const std::string ValueFile;
        const std::string DirectionFile;
        Mode m_mode;
    };
}

#endif // GPIO_H