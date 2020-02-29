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

#include "status_image.h"
#include "log.h"
#include "settings.h"
#include <nlohmann/json.hpp>
#include <pstream.h>
#include <framebuffer.h>
#include <wifi_setup.h>
#include "aquabox.h"
using namespace aquabox;

#define degree(angle) ((double)(angle) * 0.0174533)

void StatusImage::display(std::string fbDevice)
{
#ifdef TARGET_DEVICE
    IoT::FrameBuffer fb(fbDevice);

    const uint32_t margin = 5;
    if(!fb.open()) {
        LOG_ERROR << "Can't open framebuffer device " << fbDevice;
        return;
    }
    Path font = aquabox::AquaBox::i().workingDirectory();
    font.append("DroidSansMono.ttf");

    
    double temp = 0;
    {
        redi::ipstream tempCmd("cat /sys/devices/virtual/thermal/thermal_zone0/temp");
        tempCmd >> temp;
        temp /= 1000.; 
    }

    nlohmann::json host({
        {"rest", Settings::integer(Settings::Keys::HTTP_PORT, Settings::Defaults::REST_PORT)},
        {"stream", Settings::integer(Settings::Keys::STREAM_PORT, Settings::Defaults::WS_PORT)},
        {"serial", SERIAL_NUMBER}
    });

    host["host"] = boost::asio::ip::host_name();

    nlohmann::json statusJson({{"aquabox", host}});
    
    QRcode *qrcode = QRcode_encodeString(statusJson.dump(0).c_str(), 0, QR_ECLEVEL_L, QR_MODE_AN, 1);
	if (qrcode == nullptr) {
        LOG_ERROR << "Can't encode status";
        return;
    }
    
    fb.clear(0xFFFFFF);
    uint32_t qrsize = fb.drawQRCode(qrcode, margin, margin, fb.height() - margin, 0x000000);

    uint32_t widthRemain = fb.width() - qrsize - margin; 
    int FONT_SIZE = 8;

    char temperatureString[32] = "";
    memset(temperatureString, '\0', 32);
    sprintf(temperatureString, "CPU: %.1f°C", temp);

    int textX = fb.width() - 5 - FONT_SIZE;
    int textY = margin / 2;

    std::string state = "NM: Uknown";
    switch(AquaBox::i().networking().state())
    {
        case IoT::WiFi::State::Uninitialized:
        {
            state = "NM: Uninitilized";
            break;
        }
        case IoT::WiFi::State::CheckingConnectivity:
        {
            state = "NM: Checking";
            break;
        }
        case IoT::WiFi::State::InAPMode:
        {
            state = "NM: Access Point";
            break;
        }
        case IoT::WiFi::State::SwitchingToAP:
        {
            state = "NM: Switching to AP";
            break;
        }
        case IoT::WiFi::State::TryingToConnect:
        {
            state = "Connecting";
            break;
        }
        case IoT::WiFi::State::Connected:
        {
            state = "NM: OK";
            break;
        }
        default: {
            state = "NM: Err";
            break;
        }
    }


    fb.drawText(font, FONT_SIZE, textX , textY, degree(-90.), state, 0x0000FF);
    textX -= 5 + FONT_SIZE;
    std::string ssid = AquaBox::i().networking().currentSSID();
    if (ssid == "") {
        ssid = "Loading...";
    } else {
        ssid += " ";
        ssid += std::to_string(AquaBox::i().networking().wifiSignal());
        ssid += "%";
    }
    fb.drawText(font, FONT_SIZE, textX, margin, degree(-90.), ssid, 0xFF0000);
    textX -= 5 + FONT_SIZE;
    std::string ip = AquaBox::i().networking().currentIP();
    if (ip == "") {
        ip = "Loading...";
    }
    fb.drawText(font, FONT_SIZE, textX, margin, degree(-90.), ip, 0xFF0000);
    fb.flush();
#endif // TARGET_DEVICE
}