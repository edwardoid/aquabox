#ifndef GLOBAL_CONFIGS_H
#define GLOBAL_CONFIGS_H


#include <Arduino.h>

#define MOTOR_PIN D5

// [ WIFI
#define WIFI_NETWORK "Sargsyans24"
#define WIFI_PASSWORD "azazello"
#define AQUABOX_IP "192.168.1.133"
#define AQUABOX_PORT 1800
// WIFI ]

// [ UI
#define UI_ENABLED


#define USE_PCD8544
#ifdef USE_PCD8544
    #define UI_SCLK D4
    #define UI_DIN  D3
    #define UI_DC   D2
    #define UI_CS   D1
    #define UI_RST  D0
#endif // USE_PCD8544

// UI ]


enum DeviceState
{
    NotReady = -1,
    Starting = 0,
    ConnectingToNetwork = 1,
    ConnectingToAquabox = 2,
    ConnectionToAquaboxLost = 3,
    ConnectionToNetworkLost = 4,
    Working = 5,
    HandshakeFailed = 6,
    Idle
};

struct Globals
{
    static volatile DeviceState State;
};


volatile DeviceState Globals::State = DeviceState::Starting;

#endif // GLOBAL_CONFIGS_H
