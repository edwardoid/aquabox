#include "path.h"
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <memory.h>
#include <limits.h>
#include <stdlib.h>

#include "log.h"

using namespace aquabox;

Path::Path(std::string str)
    : std::string(str)
{}

std::string Path::lastToken() const
{
    auto p = find_last_of('/');
    if (p != std::string::npos)
        return substr(p + 1);
    return "";
}

bool Path::isRelative() const
{
    if(size() > 0)
    {
        return at(0) == '.' || at(0) == '~';
    }

    return false;
}

bool Path::normalize()
{
    char tmp[PATH_MAX];
    memset(tmp, 0, PATH_MAX);
    if(NULL != realpath(c_str(), tmp) || errno == ENOENT)
    {
        static_cast<std::string&>(*this) = (errno == ENOENT ? data() : tmp);
        return true;
    }
    return false;
}

bool Path::append(std::string sub)
{
    if (!sub.empty() && sub[0] != '/')
        *this += "/";
    *this += sub;
    return true;
}

bool Path::contains(const Path& subPath) const
{
    Path t = *this;
    Path s = subPath;
    t.normalize();
    s.normalize();
    return t.find(s) == 0;
}

bool Path::isDirectory() const
{
    struct stat st;
    if(stat(c_str(), &st) == 0)
        return S_ISDIR(st.st_mode);
    return false;
}

bool Path::exists() const
{
    struct stat buffer;   
    return (stat (c_str(), &buffer) == 0); 
}

std::list<Path> Path::children() const
{
    std::list<Path> paths;
    auto dir = opendir(c_str());
    if (dir != NULL)
    {
        auto desc = readdir(dir);
        while(desc != NULL)
        {
            if(desc->d_type == DT_DIR ||
               desc->d_type == DT_REG)
            {
                if(strcmp(desc->d_name, ".") != 0 &&
                   strcmp(desc->d_name, "..") != 0)
                {
                    Path p = *this;
                    p.append(desc->d_name);
                    paths.push_back(p);
                }
            }
            
            desc = readdir(dir);
        }
    }
    return paths;
}

Path Path::subPath(const Path& to) const
{
    if(contains(to))
    {
        return substr(to.size());
    }
    return Path("");
}

std::string Path::extension() const
{
    auto pos = find_last_of('.');
	auto ext = substr(pos == std::string::npos ? size() : pos +1);
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
	return ext;
}
