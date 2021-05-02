#ifndef UI_ROUTINE_H
#define UI_ROUTINE_H

#include "GlobalConfigs.h"

#ifdef UI_ENABLED

#include <Arduino.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_PCD8544.h>
#include <Scheduler.h>
#include "TimesProperty.h"

class UIRoutine : public Task
{
public:
    UIRoutine()
          : m_display(UI_SCLK, UI_DIN, UI_DC, UI_CS, UI_RST)
        { Globals::State = DeviceState::Starting; }
    protected:
        void setup()
        {
            m_display.begin();
            //m_display.setContrast(99);
            m_display.clearDisplay();
            m_display.setTextSize(1);
            m_display.setTextColor(BLACK);
            m_display.setCursor(0,0);
            m_display.println("Initializing...");
        }
        void loop()  {
            yield();
            m_display.display();
            if (LastState != Globals::State) {
                yield();
                m_display.clearDisplay();
                LastState == Globals::State;
            }
            yield();
            switch (Globals::State)
            {
                case Starting:
                {
                    
                    m_display.setCursor(0,0);
                    m_display.println("Starting");
                    break;
                }
    
                case ConnectingToNetwork:
                {
                    //display.drawBitmap(0, 0, WiFiIcon, 16, 16, BLACK, WHITE);
                    m_display.setCursor(0,0);
                    m_display.println("Connecting to network...");
                    break;
                }
    
                case ConnectingToAquabox:
                {
                    m_display.setCursor(0,0);
                    m_display.println("Connecting to AquaBox...");
                    break;
                }
    
                case ConnectionToAquaboxLost:
                {
                    m_display.setCursor(0,0);
                    m_display.println("Connection to AquaBox lost..");
                    break;
                }

                case HandshakeFailed: {
                    m_display.setCursor(0,0);
                    m_display.println("Handshake failed..");
                    break;
                }
    
                case ConnectionToNetworkLost:
                {
                  m_display.setCursor(0,0);
                  m_display.println("Disconnected");
                  break;
                }

                case Idle:
                {
                  m_display.setCursor(0,0);
                  m_display.println("Connected. Idle. Value");
                  m_display.println((int) Times::CurrentValue);
                  break;
                }
    
                case Working:
                {
                    m_display.setCursor(0,0);
                    m_display.println("Dosing ");
                    m_display.println((int) Times::CurrentValue);
                    m_display.println("ml");
                    break;
                }
            }
        }
    private:
        DeviceState LastState = DeviceState::NotReady;
        Adafruit_PCD8544 m_display;
};

#endif // UI_ENABLED

#endif // UI_ROUTINE
