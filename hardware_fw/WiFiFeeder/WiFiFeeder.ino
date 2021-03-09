
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Scheduler.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <AquaboxProto.h>


const unsigned char WiFiIcon [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x1f, 0xf8, 0x7e, 0x7e, 0x31, 0x8c, 0x0f, 0xf0, 
  0x1f, 0xf8, 0x08, 0x10, 0x03, 0xc0, 0x03, 0xc0, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00                                                                                                                                                                                                                                                                                                                
};


const aquabox::proto::byte_t FEEDER_SERIAL[SERIAL_LEN] PROGMEM = { 't', 's', 't', 'f', 'e', 'e', 'd', 'r' };

int singleShot = 500; // In microseconds


struct Times: public aquabox::proto::CachedSlaveProperty<int32_t>
{
public:
    
    static int32_t CurrentValue;

    Times() : aquabox::proto::CachedSlaveProperty<int32_t>("times", (int32_t) CurrentValue) {}
    
    virtual bool set(const aquabox::proto::ValueData& in) override
    {
        if (aquabox::proto::CachedSlaveProperty<int32_t>::set(in))
          CurrentValue = value;
        return false;
    }
    
    virtual bool get(aquabox::proto::ValueData& out) const override
    {
        if(aquabox::proto::CachedSlaveProperty<int32_t>::get(out))
          out.value.I = CurrentValue;
        return false;
    }
};

int32_t Times::CurrentValue = 0;
Times TimesProperty;
aquabox::proto::CachedSlaveProperty<uint32_t> TimesTimeoutProperty("_times", 10000);
aquabox::proto::CachedSlaveProperty<char*> ClassProperty("class", "step"), TypeProperty("type", "feeder");
aquabox::proto::CachedSlaveProperty<bool> CacheProperty("_cache", true);
aquabox::proto::CachedSlaveProperty<int32_t> PingTimeoutProperty("_timeout", 5000);

struct HermesFeeder: public aquabox::proto::EasySlave<6>
{
public:  
    static aquabox::proto::byte_t* FEEDER_TOKEN;
    static SlavePropertyPtr Props[6];
    HermesFeeder(aquabox::proto::IO* io)
      : aquabox::proto::EasySlave<6>(Props, io, FEEDER_SERIAL, FEEDER_TOKEN)
    { setProps(Props); }

    void printSerial()
    {
      Serial.println("Serial: ");
      for(int  i = 0; i < SERIAL_LEN; ++i)
        Serial.print(m_serial[i]);
    }

    virtual bool processNextMessage() override
    {
        bool getResult = false;
        aquabox::proto::Message rcv;
        getResult = m_io->get(rcv);
        //yield();
        aquabox::proto::Message rpl;
        if (dispatch(&rcv, &rpl))
        {
            yield();
            m_io->send(rpl);
            yield();
        }
        return getResult;
    }

protected:
    
};

aquabox::proto::byte_t* HermesFeeder::FEEDER_TOKEN = (aquabox::proto::byte_t*)("allow me");
HermesFeeder::SlavePropertyPtr HermesFeeder::Props[6] = { &TimesProperty, &TimesTimeoutProperty, &ClassProperty, &TypeProperty, &CacheProperty, &PingTimeoutProperty };

aquabox::proto::ArduinoStreamIO HermesIO = aquabox::proto::ArduinoStreamIO();
HermesFeeder Feeder(&HermesIO);

const char *SSID = "Sargsyans24";
const char *PASSWD = "azazello";

Adafruit_PCD8544 display(D4, D3, D2, D1, D0);
class UIRoutine : public Task
{
public:
    enum UIState
    {
        NotReady = -1,
        Starting = 0,
        ConnectingToNetwork = 1,
        ConnectingToAquabox = 2,
        ConnectionToAquaboxLost = 3,
        ConnectionToNetworkLost = 4,
        Working = 5,
        Idle
    };
  
    static volatile UIState State;
    UIRoutine()
        { State = UIState::Starting; }
    protected:
        void setup()
        {
            display.begin();
            display.setContrast(60);
            display.clearDisplay();
            display.setTextSize(1);
            display.setTextColor(BLACK);
        }
        void loop()  {
            yield();
            display.display();
            if (LastState != State) {
                yield();
                display.clearDisplay();
                LastState == State;
            }
            yield();
            switch (State)
            {
                case Starting:
                {
                    
                    display.setCursor(0,0);
                    display.println("Starting");
                    break;
                }
    
                case ConnectingToNetwork:
                {
                    //display.drawBitmap(0, 0, WiFiIcon, 16, 16, BLACK, WHITE);
                    display.setCursor(0,0);
                    display.println("Connecting to network...");
                    break;
                }
    
                case ConnectingToAquabox:
                {
                    display.setCursor(0,0);
                    display.println("Connecting to AquaBox...");
                    break;
                }
    
                case ConnectionToAquaboxLost:
                {
                    display.setCursor(0,0);
                    display.println("Connection to AquaBox lost..");
                    break;
                }
    
                case ConnectionToNetworkLost:
                {
                  display.setCursor(0,0);
                  display.println("Disconnected");
                  break;
                }

                case Idle:
                {
                  display.setCursor(0,0);
                  display.println("Connected. Idle");
                  break;
                }
    
                case Working:
                {
                    display.setCursor(0,0);
                    display.println("Dosing ");
                    display.println((int) Times::CurrentValue);
                    display.println("ml");
                    break;
                }
            }
        }
    private:
        UIState LastState = UIState::NotReady;
} UI;

class HermesIOTask : public Task {
protected:
    void setup()
    {
        UIRoutine::State = UIRoutine::UIState::Starting;
        Serial.begin(115200);
        pinMode(D5, OUTPUT);
        WiFi.mode(WIFI_STA);
        UIRoutine::State = UIRoutine::UIState::ConnectingToNetwork;
        WiFi.begin(SSID, PASSWD);
    
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

        
        UIRoutine::State = UIRoutine::UIState::ConnectingToNetwork;
        Serial.println("");
        Serial.print("Connected to ");
        Serial.println(SSID);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }
    void loop()  {
        WiFiClient client;
        UIRoutine::State = UIRoutine::UIState::ConnectingToAquabox;
        //yield();
        while(!client.connect("esargsyan-lnx", 1800)) {
            delay(1000);
            
            UIRoutine::State = UIRoutine::UIState::ConnectionToAquaboxLost;
            yield();
        }
        HermesIO.init(&client);
        Feeder.printSerial();
        if(!Feeder.handshake()) {
          UIRoutine::State = UIRoutine::UIState::ConnectionToAquaboxLost;
        }

        UIRoutine::State = UIRoutine::UIState::Idle;
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
        UIRoutine::State = UIRoutine::UIState::ConnectionToNetworkLost;
    }
} IO;


class DeviceRoutine : public Task
{
  protected:
    void setup()
    { digitalWrite(D5, LOW); }
    void loop()  {
        if (Times::CurrentValue > 0)
        {
            UIRoutine::UIState last = UIRoutine::State;
            UIRoutine::State = UIRoutine::UIState::Working;
            while(Times::CurrentValue-- > 0)
            {
                digitalWrite(D5, HIGH);
                uint32_t finishAt = singleShot * 1000 + system_get_time();
                delay(singleShot);
            }
            digitalWrite(D5, LOW);
            UIRoutine::State = last;
        }
    }
} Routine;

volatile UIRoutine::UIState UIRoutine::State = UIState::Starting;

void setup()
{
  
  UIRoutine::State = UIRoutine::UIState::Working;
    Scheduler.start(&Routine);
    Scheduler.start(&IO);
    Scheduler.start(&UI);
    Scheduler.begin();
}

// the loop function runs over and over again forever
void loop() {}
