#ifndef GPIO_H
#define GPIO_H

#include "hardware.h"
#include <memory>
#include <string>

namespace aquabox
{
    class GPIO: public Hardware
    {
    public:
        GPIO(std::string pin);
        enum class Mode
        {
            Unknown,
            In,
            Out
        };
    private:
        Mode readMode();
        bool writeMode(Mode mode);
        bool exported() const;
    public:
        bool open() override;
        bool close() override;
        bool isHigh();
        BytesCount send(const Bytes& data, const BytesCount count = 0) override;
        BytesCount read(Bytes& data, const Offset = 0) override;
    private:
        static const std::string SysClassGpio;
        static const std::string SysClassGpioExport;
        static const std::string SysClassGpioUnexport;
        std::string Pin;
        const std::string ValueFile;
        const std::string DirectionFile;
        Mode m_mode;
    };
}

#endif // GPIO_H