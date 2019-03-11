#ifndef SPI_H
#define SPI_H

#include "protocol.h"

namespace aquabox
{
    class SPI: public Protocol
    {
    public:
        SPI();
        void send(const Bytes& data, const BytesCount count = 0) override;
        BytesCount read(Bytes& data, const Offset = 0) override;
    };
}


#endif SPI_H