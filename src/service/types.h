#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <vector>

namespace aquabox
{

struct Bytes: public std::vector<uint8_t>
{};

using BytesCount = Bytes::size_type;
using Offset = size_t;


}

#endif // TYPES_H