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
 * along with this program.  If not, see <http://www.gnm_main
 * m_mainu.org/licenses/>.
 */

#include "screen_manager.h"
#include <chrono>
#include <common/log.h>
#include <micro_clients/settings.h>
#include <thread>

using namespace aquabox;

ScreenManager::ScreenManager()
{
    init();
}

void ScreenManager::show()
{
    auto fb = Singleton<LIBFB_NS::FrameBuffer>::i();
    if (fb == nullptr) {
        return;
    }

    if (m_networkChangeScreen.hasUpdate()) {
        m_networkChangeScreen.show();
    }
    else if (false /* m_cloudScreen.hasUpdate() */) {
        //m_cloudScreen.show();
    }
    else {
        static std::vector<Screen*> allScreens({&m_main, &m_scan, &m_stats, &m_stats});
        static uint32_t nextScreen = 0;
        static const std::chrono::seconds singleScreenDuration = std::chrono::seconds(10);
        static std::chrono::system_clock::time_point begin = std::chrono::system_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - begin) >= singleScreenDuration) {
            ++nextScreen;
            begin = std::chrono::system_clock::now();
        }
        allScreens[nextScreen % allScreens.size()]->show();
    }

    fb->flush();
}

void ScreenManager::init()
{
    auto fb = Singleton<LIBFB_NS::FrameBuffer>::i();
    if (fb == nullptr) {
        return;
    }

    auto dev = Singleton<ISettings>::i()->path(Settings::Keys::FRAMEBUFFER, Settings::Defaults::FRAMEBUFFER);

    if (boost::filesystem::status(dev).type() != boost::filesystem::file_type::character_file) {
        LOG_INFO << "Framebuffer device " << dev << " does not exist or is not a framebuffer!";
        exit(EXIT_FAILURE);
    }
    if (!fb->open(dev.c_str())) {
        Singleton<LIBFB_NS::FrameBuffer>::destroy();
    }
}
