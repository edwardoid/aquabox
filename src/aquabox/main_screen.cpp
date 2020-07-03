#include "main_screen.h"
#include <ctime>

#define degree(angle) ((double)(angle) * 0.0174533)

using namespace aquabox;

MainScreen::MainScreen(std::string dev)
    : StatusBarScreen(dev)
{
    m_text.setColor(0x5aa61aff);
    m_text.setSize(24);
    m_text.setFont(font("DroidSansMono"));
}

void MainScreen::draw()
{
    StatusBarScreen::draw();

    std::time_t rawtime;
    std::time (&rawtime);
    std::tm* timeinfo = std::localtime (&rawtime);

    std::string time = std::to_string(timeinfo->tm_hour)
                        + std::string(timeinfo->tm_sec % 2 ? ":" : " ")
                        + std::to_string(timeinfo->tm_min / 10)
                        + std::to_string(timeinfo->tm_min % 10);
    
    m_text.setText(time);



    m_text.mapTo((fb().width() - m_text.width()) / 2, 
                 (fb().height() - m_text.height()) / 2 + m_text.height() / 2);

    fb().draw(&m_text);
}