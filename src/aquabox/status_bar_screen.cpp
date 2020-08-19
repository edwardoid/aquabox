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
#include "aquabox.h"
#include "cloud.h"
#include "network_manager.h"
#include "rotate.h"
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

void StatusBarScreen::draw()
{
    std::vector<int> elements;

    int wifiIndex = 10;
    if (NetworkManager::i().currentConnection.value.signal > 0) {
        wifiIndex = NetworkManager::i().currentConnection.value.signal / 25;
    }

    elements.push_back(wifiIndex);

    int cloudIndex = AquaBox::i().cloud().isRegistered()
                    ? 6
                    : 7;
    elements.push_back(cloudIndex);


    int assistantIndex = AquaBox::i().assistantIsConnected() ? 8
                                                             : 9;

    elements.push_back(assistantIndex);

    LIBFB_NS::Sprite statusBar(pixmap("status_bar"), 24, 24, 10);

    fb().fillRect(0, 0, fb().width(), 24, 0x81bf2a);
    fb().drawLine(0, 24, fb().width(), 24, 0xebeb8c);
    
    int spacing = (fb().width() - 24 * elements.size()) / (elements.size() + 1);

    for(int i = 0; i < elements.size(); ++i)
    {
        statusBar.jump(elements[i]);
        statusBar.mapTo(i * (24 + spacing) + spacing, 0);
        fb().draw(&statusBar);
    }
}