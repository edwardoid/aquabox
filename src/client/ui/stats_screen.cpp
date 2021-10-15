/*
 * AquaBox
 * Copyright (C) 2020  Eduard Sargsyan
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

#include "stats_screen.h"

#include <common/performance_metrics.h>
#include <common/singleton.h>
#include <sstream>

using namespace aquabox;

StatsScreen::StatsScreen()
{
    m_uptime.setColor(0x000000ff);
    m_uptime.setSize(8);
    m_uptime.setFont(font("DroidSansMono"));
    m_uptime.setAngle(0.);
}

void StatsScreen::draw()
{
    auto fb = Singleton<LIBFB_NS::FrameBuffer>::i();
    if (fb == nullptr) {
        return;
    }

    StatusBarScreen::draw();

    auto u = PerformanceMetrics::uptime();

    std::stringstream ss;
    ss << "Uptime";
    std::chrono::hours h = std::chrono::duration_cast<std::chrono::hours>(u);
    std::chrono::minutes m = std::chrono::duration_cast<std::chrono::minutes>(u);
    std::chrono::seconds s = std::chrono::duration_cast<std::chrono::seconds>(u);

    if (h.count() / 24 > 0) {
        ss << " " << h.count() / 24 << "d";
    }

    if (h.count() % 24 > 0) {
        ss << " " << h.count() % 24 << "h";
    }

    if (m.count() % 60 > 0) {
        ss << " " << m.count() % 60 << "m";
    }

    ss << " " << s.count() % 60 << "s";

    m_uptime.setText(ss.str());

    int offsetY = ((int)fb->height() - (int)StatusBarScreen::height() - (int)m_uptime.height()) / 2;
    offsetY = std::max<int>(0, offsetY);
    int offsetX = ((int)fb->width() - (int)m_uptime.width()) / 2;
    offsetX = std::max(offsetX, 0);
    m_uptime.mapTo(offsetX, StatusBarScreen::height() + offsetY);

    fb->draw(&m_uptime);
}