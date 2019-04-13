#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "types.h"

namespace aquabox
{
    class Protocol
    {
        public:
        virtual bool open() = 0;
        virtual bool close() = 0;
        virtual BytesCount send(const Bytes& data, const BytesCount count = 0) = 0;
        virtual BytesCount read(Bytes& data, const Offset = 0) = 0;
    };
}

#endif // PROTOCOL