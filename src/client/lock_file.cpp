/*
 * AquaBox
 * Copyright (C) 2019  Eduard Sargsyan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "lock_file.h"
#include <common/log.h>
#include <micro_clients/settings.h>
#include <sys/file.h>
#include <unistd.h>

using namespace aquabox;

#define _R (1 << 2) // Read
#define _W (1 << 1) // Write
#define _X (1 << 0) // Execute

#define _RW _R | _W
#define _RWX RW | _X

#define _A(mode) (mode) // All
#define _G(mode) ((mode) << 3) // Group
#define _O(mode) ((mode) << 6) // Owner

#define LOCK_FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

Lockfile::Lockfile()
{
}

bool Lockfile::create()
{
    if (m_fd > 0)
        remove();

    m_fd = open(Settings::Defaults::LOCK_FILE.c_str(),
        O_CREAT | O_WRONLY | O_EXCL, LOCK_FILE_MODE);
    return m_fd > 0;
}

void Lockfile::remove()
{
    if (m_fd > -1) {
        close(m_fd);
    }

    int ret = ::remove(Settings::Defaults::LOCK_FILE.c_str());
    LOG_FATAL_IF(ret != 0) << "Can't delete lock file "
                           << Settings::Defaults::LOCK_FILE;

    m_fd = -1;
}

Lockfile::~Lockfile() { remove(); }