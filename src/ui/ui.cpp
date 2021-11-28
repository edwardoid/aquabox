/*
 * AquaBox
 * Copyright (C) 2019-2020  Eduard Sargsyan
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


#include "ui.h"
#include "screen_manager.h"
#include <micro_clients/settings.h>
#include <common/log.h>
#include <thread>
#include <chrono>

using namespace aquabox;

UI::UI()
    : Application("UI")
{}

int UI::start()
{
    try
    {
        int fps = Singleton<ISettings>::i()->get(Settings::Keys::FPS, Settings::Defaults::FPS);
        if (fps < 1) { fps = 1; };
        std::chrono::milliseconds fpsTimeout(1000 / fps);
        LOG_INFO << "Desired fps is " << fps << " changed to ~" << (1000 / fpsTimeout.count()) << " updating screen each " << fpsTimeout.count() << "ms.";
        while(1)
        {
            std::chrono::high_resolution_clock::time_point before = std::chrono::high_resolution_clock::now();
            Singleton<ScreenManager>::i()->show();
            std::chrono::high_resolution_clock::time_point after = std::chrono::high_resolution_clock::now();
            std::chrono::high_resolution_clock::duration delta = after - before;
            if (delta > fpsTimeout) {
                std::chrono::milliseconds renderTime = std::chrono::duration_cast<std::chrono::milliseconds>(delta);
                LOG_INFO << "Dropped " << (delta / fpsTimeout) << " frames. Rendering took " << renderTime.count() << "ms." << " throttling " << renderTime.count() / fpsTimeout.count() << " frames.";
                before = after - std::chrono::milliseconds(delta.count() % fpsTimeout.count());
            }
            std::this_thread::sleep_until(before + fpsTimeout);
        }
    }
    catch(...) {
        return EXIT_FAILURE;
    }

    return Application::start();
}