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
        while(1)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            Singleton<ScreenManager>::i()->show();
        }
    }
    catch(...) {
        return EXIT_FAILURE;
    }

    return Application::start();
}