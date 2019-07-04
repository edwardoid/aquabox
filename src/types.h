#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <vector>

namespace aquabox
{

using Bytes = std::vector<uint8_t>;
using BytesCount = Bytes::size_type;
using Offset = Bytes::size_type;


}

#endif // TYPES_H