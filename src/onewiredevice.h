#include "hardware.h"
#include "ow_device.h"

namespace aquabox
{
    class OneWireDevice: public Hardware
    {
    public:
        OneWireDevice(const w1::Device& dev);
    public:
        bool open(int flags) override;
        bool close() override;
        BytesCount send(const Bytes& data, const BytesCount count = 0) override;
        BytesCount read(Bytes& data, const Offset = 0) override;
    private:
        w1::Device m_dev;
    };
}