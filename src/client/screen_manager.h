/*
 * AquaBox
 * Copyright (C) 2020 Eduard Sargsyan
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

#pragma once
#include "scan_screen.h"
#include "main_screen.h"
#include "stats_screen.h"
#include "network_change_screen.h"
#include <singleton.h>
namespace aquabox
{
    class ScreenManager
    {
    protected:
        ScreenManager();
        friend Singleton<ScreenManager>;
    public:
        void show();
    private:
        void init();
    private:
        MainScreen m_main;
        ScanScreen m_scan;
        StatsScreen m_stats;
        NetworkChangeScreen m_networkChangeScreen;
    };
}
