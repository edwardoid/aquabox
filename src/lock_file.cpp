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


#define LOCK_FILE_MODE  (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

Lockfile::Lockfile()
{
}

bool Lockfile::create()
{
    if (m_fd > 0)
        remove();

    m_fd = open(Config::LOCK_FILE.c_str(), O_CREAT | O_WRONLY | O_EXCL, LOCK_FILE_MODE);
    return m_fd > 0;
}

void Lockfile::remove()
{
    if (m_fd > -1) {
        close(m_fd);        
    }

    int ret = ::remove(Config::LOCK_FILE.c_str());
    LOG_FATAL_IF(ret != 0) << "Can't delete lock file " << Config::LOCK_FILE;

    m_fd = -1;
}

Lockfile::~Lockfile()
{
    remove();
}