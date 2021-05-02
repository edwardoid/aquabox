
#include <Arduino.h>
#include "GlobalConfigs.h"
#include "UIRoutine.h"
#include "TimesProperty.h"
#include "HermesFeeder.h"
#include "HermesIOTask.h"
#include "DeviceRoutine.h"
#include <Scheduler.h>
#include <AquaboxProto.h>


const unsigned char WiFiIcon [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x1f, 0xf8, 0x7e, 0x7e, 0x31, 0x8c, 0x0f, 0xf0, 
  0x1f, 0xf8, 0x08, 0x10, 0x03, 0xc0, 0x03, 0xc0, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00                                                                                                                                                                                                                                                                                                                
};

#ifdef UI_ENABLED
UIRoutine UI;
#endif // UI_ENABLED

HermesIOTask IO;
DeviceRoutine Routine;

void setup()
{
    Globals::State = DeviceState::Working;
    Scheduler.start(&Routine);
    Scheduler.start(&IO);

#ifdef UI_ENABLED
    Scheduler.start(&UI);
#endif UI_ENABLED
    Scheduler.begin();
}

// the loop function runs over and over again forever
void loop() {}
