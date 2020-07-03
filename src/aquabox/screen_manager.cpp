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
#include "settings.h"
#include "log.h"
#include <thread>

using namespace aquabox;


ScreenManager::ScreenManager()
    : m_main(Settings::string(Settings::Keys::FRAMEBUFFER, Settings::Defaults::FRAMEBUFFER))
    , m_scan(Settings::string(Settings::Keys::FRAMEBUFFER, Settings::Defaults::FRAMEBUFFER))
    , m_network(Settings::string(Settings::Keys::FRAMEBUFFER, Settings::Defaults::FRAMEBUFFER))
{
    init();
}

void ScreenManager::show()
{
    if((time(NULL) % 60) > 30) {
        m_main.show();
    } else {
        m_scan.show();
    }
}

void ScreenManager::init()
{
    static std::thread gui([this] {
        while(true) {
            show();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } );

    gui.detach();
}
