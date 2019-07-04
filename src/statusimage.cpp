#include "statusimage.h"
#include "log.h"
#include "settings.h"
#include "config.h"
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
        {"rest", Settings::integer(Settings::HTTP_PORT, Config::REST_PORT)},
        {"stream", Settings::integer(Settings::STREAM_PORT, Config::WS_PORT)}
    });

    host["host"] = boost::asio::ip::host_name();

    nlohmann::json statusJson({{"aquabox", host}});
    
    QRcode *qrcode = QRcode_encodeString(statusJson.dump(0).c_str(), 0, QR_ECLEVEL_L, QR_MODE_8, 1);
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
    int textY = margin;
    fb.drawText(font, FONT_SIZE, textX , textY, degree(-90.), temperatureString, 0xDDDD00);
    textX -= 5 + FONT_SIZE;
    std::string ssid = AquaBox::i().networking().currentSSID();
    if (ssid == "") {
        ssid = "Loading...";
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