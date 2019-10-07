#ifndef MOCK_HARDWARE_H
#define MOCK_HARDWARE_H

#include "hardware.h"
#include <string>

namespace aquabox
{
    class MockHardware: public Hardware
    {
    public:
        MockHardware(std::string identifier);
    public:
        bool open(int flags) override;
        bool close() override;
        BytesCount send(const Bytes& data, const BytesCount count = 0) override;
        BytesCount read(Bytes& data, const Offset = 0) override;
    private:
        void writeMockData();
        void readMockData(bool force = false);
    private:
        std::string m_identifier;
        bool m_opened = false;
        int m_flags = 0;
        Bytes m_data;
    };
}

#endif // GPIO_H