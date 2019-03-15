#ifndef PATH_H
#define PATH_H

#include <string>

namespace aquabox
{
    class Path: public std::string
    {
    public:
        bool isRelative() const;
        bool normalize();
        bool append(std::string sub);
    };
}

#endif // PATH_H