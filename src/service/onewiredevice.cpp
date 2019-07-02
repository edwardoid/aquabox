#include "onewiredevice.h"

using namespace aquabox;

OneWireDevice::OneWireDevice(const w1::Device& dev)
    : m_dev(dev)
{}

bool OneWireDevice::open(int)
{
    return true;
}

bool OneWireDevice::close()
{
    return true;
}

BytesCount OneWireDevice::send(const Bytes& data, const BytesCount count)
{
    const int prec = atoi(reinterpret_cast<const char*>(data.data()));
    m_dev.setPrecition(static_cast<w1::Precision>(prec));
    return data.size();
}

BytesCount OneWireDevice::read(Bytes& data, const Offset)
{
    auto temp = std::to_string((int)(m_dev.temperature() * 1000));
    data.resize(temp.size() + 1);
    data.assign(temp.begin(), temp.end());
    return temp.size();
}