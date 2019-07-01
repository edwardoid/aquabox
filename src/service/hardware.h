#ifndef HARDWARE_H
#define HARDWARE_H

#include "types.h"
#include <memory>

namespace aquabox
{
    class Hardware
    {
    public:
        static std::shared_ptr<Hardware> resolve(const std::string& address);
        virtual bool open() = 0;
        virtual bool close() = 0;
        virtual BytesCount send(const Bytes& data, const BytesCount count = 0) = 0;
        virtual BytesCount read(Bytes& data, const Offset = 0) = 0;
    };
}

#endif // HARDWARE_H