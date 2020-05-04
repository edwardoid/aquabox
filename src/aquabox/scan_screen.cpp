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
#include "log.h"
#include "settings.h"
#include <nlohmann/json.hpp>
#include <framebuffer.h>
#include <pngprovider.h>
#include <sprite.h>
#include "aquabox.h"
using namespace aquabox;

#define degree(angle) ((double)(angle) * 0.0174533)

#define QR_X 2
#define QR_Y 2
#define QR_SIZE 124

void ScanScreen::draw()
{
    StatusBarScreen::draw();
    
    QRcode *qrcode = QRcode_encodeString(AquaBox::i().configurationData().c_str(), 0, QR_ECLEVEL_L, QR_MODE_8, 1);
	if (qrcode == nullptr) {
        LOG_ERROR << "Can't encode status";
        return;
    }

    std::string temp = std::to_string(AquaBox::i().systemTemperature());
    temp += "°C";

#ifdef TARGET_DEVICE
    auto estimatedQrSize = fb().qrCodeSize(qrcode, QR_SIZE);
    uint32_t qrsize = fb().drawQRCode(qrcode,
        (fb().width() - 29 - estimatedQrSize) / 2,
        (fb().height() - estimatedQrSize ) / 2,
        QR_SIZE, 0x000000);
#endif // TARGET_DEVICE
}
