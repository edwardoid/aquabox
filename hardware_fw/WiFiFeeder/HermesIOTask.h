#ifndef HERMES_IO_TASK_H
#define HERMES_IO_TASK_H

#include <Scheduler.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "GlobalConfigs.h"


class HermesIOTask : public Task {
protected:
    void setup()
    {
        Globals::State = DeviceState::Starting;
        Serial.begin(115200);
        pinMode(D5, OUTPUT);
        WiFi.mode(WIFI_STA);
        Globals::State = DeviceState::ConnectingToNetwork;
        WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);
    
        WiFi.hostname("HermesFeeder");
    
        int it = 0;
        Serial.println("Connecting to WiFi");
        
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(50);
            digitalWrite(LED_BUILTIN, it++ % 2 ? HIGH : LOW);
            Serial.print('.');
        }
        digitalWrite(LED_BUILTIN, LOW);

        
        Globals::State = DeviceState::ConnectingToNetwork;
        Serial.println("");
        Serial.print("Connected to ");
        Serial.println(WIFI_NETWORK);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }
    void loop()  {
        WiFiClient client;
        Globals::State = DeviceState::ConnectingToAquabox;
        //yield();
        while(!client.connect(AQUABOX_IP, AQUABOX_PORT)) {
            delay(1000);
            
            Globals::State = DeviceState::ConnectionToAquaboxLost;
            yield();
        }
        HermesIO.init(&client);
        if(!Feeder.handshake()) {
          Globals::State = DeviceState::HandshakeFailed;
        }

        Globals::State = DeviceState::Idle;
        while(1) {
          if (HermesIO.available() >= sizeof(aquabox::proto::Message)) {
            if(Feeder.processNextMessage()) {
              yield();
            }
          } else {
            HermesIO.wait(sizeof(aquabox::proto::Message));
            yield();
          }
        }

        client.stop();
        Globals::State = DeviceState::ConnectionToNetworkLost;
    }
};

#endif // HERMES_IO_TASK_H
