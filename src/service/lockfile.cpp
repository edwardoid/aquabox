#include "lockfile.h"
#include "log.h"
#include "config.h"
#include <unistd.h>
#include <sys/file.h>

using namespace aquabox;

Lockfile::Lockfile()
{
    m_fd = open(Config::LOCK_FILE.c_str(), O_CREAT);
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