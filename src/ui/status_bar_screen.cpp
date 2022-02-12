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

#include "status_bar_screen.h"
#include <common/singleton.h>
#include <micro_clients/cloud.h>
#include <micro_clients/network_manager.h>
#include <rotate.h>
#include <sprite.h>

#define ASSISTANT_X 132
#define ASSISTANT_Y 97

#define CLOUD_X 131
#define CLOUD_Y 66

#define HOTSPOT_X 134
#define HOTSPOT_Y 38

#define WIFI_X 134
#define WIFI_Y 8

using namespace aquabox;

std::size_t StatusBarScreen::height() const
{
    return 24;
}

void StatusBarScreen::draw()
{
    auto fb = Singleton<LIBFB_NS::FrameBuffer>::i();
    if (fb == nullptr) {
        return;
    }

    std::vector<int> elements;

    int wifiIndex = 10;
    int signalStrength = Singleton<INetworkManager>::i()->currentNetwork().strength;
    if (signalStrength > 0) {
        wifiIndex = signalStrength / 25;
    }

    elements.push_back(wifiIndex);

    int cloudIndex = 7; // Singleton<ICloud>::i()->connected() ? 7 : 6;
    elements.push_back(cloudIndex);

    int assistantIndex = /*Singleton<AquaBox>::get()->assistantIsConnected()*/ false ? 8 : 9;

    elements.push_back(assistantIndex);

    LIBFB_NS::Sprite statusBar(pixmap("status_bar"), 24, 24, 10);

    fb->fillRect(0, 0, fb->width(), 24, 0x81bf2a);
    fb->drawLine(0, height(), fb->width(), 24, 0xebeb8c);

    static const uint32_t live[] = {0xff0000, 0x00ff00, 0x0000ff};

    fb->fillRect(0, 0, 5, 5, live[time(NULL) % 3]);

    int spacing = (fb->width() - 24 * elements.size()) / (elements.size() + 1);

    for (int i = 0; i < elements.size(); ++i) {
        statusBar.jump(elements[i]);
        statusBar.mapTo(i * (24 + spacing) + spacing, 0);
        fb->draw(&statusBar);
    }
}