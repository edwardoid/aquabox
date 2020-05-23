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

#include "status_bar_screen.h"
#include "aquabox.h"
#include "cloud.h"
#include "network_manager.h"
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
    int wifiIndex = 10;
    if (NetworkManager::i().currentNetwork.value.strength > 0) {
        wifiIndex = NetworkManager::i().currentNetwork.value.strength / 25;
    }

    int cloudIndex = AquaBox::i().cloud().isRegistered()
                    ? 13
                    : 12;


    int assistantIndex = AquaBox::i().assistantIsConnected() ? 15
                                                             : 14;

#ifdef TARGET_DEVICE
    IoT::Sprite statusBar(pixmap("status_bar"), 25, 25, 18);

    statusBar.jump(wifiIndex);
    fb().drawPixmap(WIFI_X, WIFI_Y, &statusBar);

    statusBar.jump(cloudIndex);
    fb().drawPixmap(CLOUD_X, CLOUD_Y, &statusBar);
    
    statusBar.jump(11);
    fb().drawPixmap(HOTSPOT_X, HOTSPOT_Y, &statusBar);

    statusBar.jump(assistantIndex);
    fb().drawPixmap(ASSISTANT_X, ASSISTANT_Y, &statusBar);
#endif // TARGET_DEVICE
}