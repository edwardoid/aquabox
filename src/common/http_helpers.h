#ifndef HTTP_HELPERS_H
#define HTTP_HELPERS_H

#include <fstream>
#include "api.h"
#include "path.h"

namespace aquabox
{
    class HttpHelpers
    {
    public:
    static void serveFile(v1::API::Response response, Path path, v1::API::Headers& headers, bool removeWhenFinished);
    static void serveStream(v1::API::Response response, Path path, std::shared_ptr<std::ifstream> ifs, size_t length, bool removeWhenFinished);
    };
}

#endif // HTTP_HELPERS_H