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

#include "scan_screen.h"
#include <log.h>
#include <settings.h>
#include <nlohmann/json.hpp>
#include <framebuffer.h>
#include <pngprovider.h>
#include <sprite.h>
#include <singleton.h>
#include "../aquabox.h"
#include <qrcode.h>
#include <libfb_globals.h>
using namespace aquabox;

#define degree(angle) ((double)(angle) * 0.0174533)

#define QR_X 2
#define QR_Y 2
#define QR_SIZE 124

void ScanScreen::draw()
{
    auto fb = Singleton<LIBFB_NS::FrameBuffer>::i();
    if (fb == nullptr) {
        return;
    }

    StatusBarScreen::draw();

    int margins = 2;
    int canvasWidth = fb->width() - 2 * margins;
    int canvasHeight = fb->height() - (StatusBarScreen::height() + 2 * margins);

    LIBFB_NS::QRCode code;
    code.setText(Singleton<AquaBox>::get()->configurationData());
    code.setColors(
        0x000000FF,
        0xFFFFFFFF
    );
    code.resizeTo(std::min(canvasWidth, canvasHeight));

    std::string temp = std::to_string(Singleton<AquaBox>::get()->systemTemperature());
    temp += "°C";

    code.mapTo((canvasWidth - code.width()) / 2,
               (margins + StatusBarScreen::height()) + (canvasHeight - code.height()) / 2);

    fb->draw(&code);
}
