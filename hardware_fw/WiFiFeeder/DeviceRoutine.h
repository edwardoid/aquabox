#ifndef DEVICE_ROUTINE_H
#define DEVICE_ROUTINE_H

#include <Arduino.h>
#include "GlobalConfigs.h"
#include "TimesProperty.h"
#include <Scheduler.h>


#define SINGLE_SHOT_DELAY 100 // In microseconds

class DeviceRoutine : public Task
{
  protected:
    void setup() {
        pinMode(MOTOR_PIN, OUTPUT);
        digitalWrite(MOTOR_PIN, LOW);
    }
    void loop()  {
        if (Times::CurrentValue > 0)
        {
            DeviceState last = Globals::State;
            Globals::State = DeviceState::Working;
            uint32_t EndAt = millis() + SINGLE_SHOT_DELAY * Times::CurrentValue;
            while(Times::CurrentValue-- > 0)
            {
                digitalWrite(MOTOR_PIN, HIGH);
                delay(SINGLE_SHOT_DELAY);
                TimesProperty.update();
                yield();
                Serial.print(" -> ");
                Serial.println(Times::CurrentValue);
            }
            Times::CurrentValue = 0;
            digitalWrite(MOTOR_PIN, LOW);
            TimesProperty.update();
            Globals::State = last;
        }
    }
};


#endif // DEVICE_ROUTINE_H
