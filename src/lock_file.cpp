#include "lock_file.h"
#include "log.h"
#include "config.h"
#include <unistd.h>
#include <sys/file.h>

using namespace aquabox;

#define _R ( 1 << 2 ) // Read
#define _W ( 1 << 1 ) // Write
#define _X ( 1 << 0 ) // Execute

#define _RW _R | _W
#define _RWX RW | _X

#define _A(mode) (mode)         // All
#define _G(mode) ((mode) << 3 ) // Group
#define _O(mode) ((mode) << 6 ) // Owner


#define LOCK_FILE_MODE  _O(_RW) | _G(_R) | _A(_R)

Lockfile::Lockfile()
{
    m_fd = open(Config::LOCK_FILE.c_str(), O_CREAT, LOCK_FILE_MODE);
    if (m_fd != -1)
    {
        int res = flock(m_fd, LOCK_EX);
        if (res != 0)
        {
            close(m_fd);
            m_fd = -1;
        }
    }
}

bool Lockfile::exists()
{
    return m_fd == -1;
}

Lockfile::~Lockfile()
{
    if (m_fd != -1)
    {
        int res = flock(m_fd, LOCK_UN);
        close(m_fd);
        int ret = remove(Config::LOCK_FILE.c_str());
        LOG_FATAL_IF(ret != 0) << "Can't delete lock file " << Config::LOCK_FILE;
    }
}