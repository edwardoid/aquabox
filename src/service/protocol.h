#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "identifiable.h"
#include "types.h"

namespace aquabox
{
    class Protocol: public Identifiable
    {
        public:
        Protocol() = default;
        Protocol(const Protocol&) = default;
        Protocol(Protocol&&) = default;
        virtual void send(const Bytes& data, const BytesCount count = 0) = 0;
        virtual BytesCount read(Bytes& data, const Offset = 0) = 0;
    };
}

#endif // PROTOCOL