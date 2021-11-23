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

#include "aquabox.h"
#include "cloud.h"
#include "options.h"
#include <micro_clients/settings.h>
#include <common/performance_metrics.h>
#include <common/singleton.h>
// #include <common/stacktrace.h>
#include <iostream>

int main(int argc, char** argv)
{
    std::string procPath = "/proc/";
    procPath += std::to_string(static_cast<int>(getpid()));
    procPath += "/exe";

    char exePath[1024];
    memset(exePath, '\0', 1024);

    int bytes = std::min<size_t>(readlink(procPath.c_str(), exePath, 1024), 1024 - 1);
    if (bytes >= 0) {
        exePath[bytes] = '\0';
    }

    std::string cwd = exePath;

    auto pos = cwd.find_last_of('/');
    if (pos > 0)
        cwd.erase(pos);

    chdir(cwd.c_str());

    aquabox::Settings::setDomain("aquabox");

    bool handleSignals = true;

    aquabox::PerformanceMetrics::uptime();

    aquabox::Singleton<aquabox::AquaBox>::get()->addOption("standalone", "Disables Cloud support");
    aquabox::Singleton<aquabox::AquaBox>::get()->addOption("help", "Show this help");
    return aquabox::Singleton<aquabox::AquaBox>::get()->exec(argc, argv);
}