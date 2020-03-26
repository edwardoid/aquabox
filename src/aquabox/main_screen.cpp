#include "main_screen.h"
#include <ctime>

#define degree(angle) ((double)(angle) * 0.0174533)

using namespace aquabox;

void MainScreen::draw()
{
    StatusBarScreen::draw();

    std::time_t rawtime;
    std::time (&rawtime);
    std::tm* timeinfo = std::localtime (&rawtime);

    std::string time = std::to_string(timeinfo->tm_hour)
                        + std::string(":")
                        + std::to_string(timeinfo->tm_min);

    uint32_t estimatedWidth = 0, estimatedHeight = 0;
    fb().textSize(font("DroidSansMono"), 24, time, 0, estimatedWidth, estimatedHeight);
    fb().drawText(font("DroidSansMono"), 24, (fb().width() - estimatedWidth)/2, (fb().height() - estimatedHeight)/2, degree(90), time, 0xFF);
}