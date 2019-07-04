#ifndef GPIO_H
#define GPIO_H

#include "hardware.h"
#include <string>

namespace aquabox
{
    class GPIO: public Hardware
    {
    public:
        GPIO(std::string pin);
        enum class Mode
        {
            Unknown = 0,
            In = 1,
            Out = 2
        };
    private:
        Mode readMode();
        bool writeMode(Mode mode);
        bool exported() const;
    public:
        bool open(int flags) override;
        bool close() override;
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