#include "statusimage.h"
#include "networkmanager.h"
#include "framebuffer.h"
#include "settings.h"
#include "config.h"
#include <nlohmann/json.hpp>
#include <pstream.h>
#include "aquabox.h"
using namespace aquabox;

#define degree(angle) ((double)(angle) * 0.0174533)

void StatusImage::display(std::string fbDevice)
{
#ifdef TARGET_DEVICE
    FrameBuffer fb(fbDevice);

    const uint32_t margin = 5;
    if(!fb.open())
        return;
    Path font = aquabox::AquaBox::i().workingDirectory();
    font.append("DroidSansMono.ttf");

    
    double temp = 0;
    {
        redi::ipstream tempCmd("cat /sys/devices/virtual/thermal/thermal_zone0/temp");
        tempCmd >> temp;
        temp /= 1000.; 
    }

    auto interfaces = NetworkManager::interfaces();

    nlohmann::json host({
        {"rest", Settings::integer(Settings::HTTP_PORT, Config::REST_PORT)},
        {"stream", Settings::integer(Settings::STREAM_PORT, Config::WS_PORT)}
    });

    host["host"] = boost::asio::ip::host_name();

    nlohmann::json statusJson({{"aquabox", host}});
    
    QRcode *qrcode = QRcode_encodeString(statusJson.dump(0).c_str(), 0, QR_ECLEVEL_L, QR_MODE_8, 1);
	if (qrcode == nullptr)
        return;
    
    fb.clear(0xFFFFFF);
    uint32_t qrsize = fb.drawQRCode(qrcode, margin, margin, fb.height() - margin, 0x000000);

    LOG_DEBUG << "QR size" << qrsize; 

    int linesCount = 1;
    for (auto& iface: interfaces) {
        if (!iface.connected)
            continue;
        ++linesCount;
    }

    uint32_t widthRemain = fb.width() - qrsize - margin; 
    int FONT_SIZE = 8;

    char temperatureString[32] = "";
    memset(temperatureString, '\0', 32);
    sprintf(temperatureString, "CPU: %.1f°C", temp);

    int textX = fb.width() - 5 - FONT_SIZE;
    int textY = margin;
    fb.drawText(font, FONT_SIZE, textX , textY, degree(-90.), temperatureString, 0xDDDD00);

    for (auto& iface: interfaces)
    {
        if (!iface.connected)
            continue;

        std::string type = iface.device == "wifi" ? "WiFi " : iface.device;
        type = type == "ethernet" ? "Wired" : type;

        textX -= FONT_SIZE + 5;
        type += iface.ip;
        LOG_DEBUG << "Drawing " << type << "at x = " << textX << " y = " << margin << " with FS = " << FONT_SIZE;
        fb.drawText(font, FONT_SIZE, textX, margin, degree(-90.), type, 0xFF0000);
    }
    #endif // TARGET_DEVICE
}