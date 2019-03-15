#ifndef PATH_H
#define PATH_H

#include <string>
#include <list>

namespace aquabox
{
    class Path: public std::string
    {
    public:
        Path(std::string str = "");
        std::string lastToken() const;
        bool isRelative() const;
        bool normalize();
        bool append(std::string sub);
        bool isDirectory() const;
        std::list<Path> children() const;
        bool contains(const Path& subPath) const;
        Path subPath(const Path& to) const;
    };
}

#endif // PATH_H