#include "IMU.h"
#include "SMP.h"
#include "NetworkManager.h"
#include "DebugMsg.h"
#include "Sounds.h"
#include "modeManager.h"
#include "String"
#include <ArduinoJson.h>

IMU Imu;
SMP smp;
Sounds buzzer;

DebugMsgs debug_msgs("main.cpp");

auto& network_manager = NetworkManager::getInstance(); 
uint8_t mac[6] = {0xF8, 0xB3, 0xB7, 0x20, 0x31, 0x18};

const int ADCpin = 36;

//--------------------------Timers----------------------------
unsigned long noneTimer;
unsigned long forwardTimer;
unsigned long backwardTimer;

const char* ssid;
const char* password;

StaticJsonDocument<1024> doc;

float GetBatery()
{
    int raw = analogRead(ADCpin);
    float v = (raw / 4095.0) * 3.3 *2;
    return v;
}

void loadValues(StaticJsonDocument<1024>& d)
{
    File file = LittleFS.open("/config.json", "r");
    deserializeJson(d, file);
    file.close();
}

void setup() {

    buzzer.begin(17);
    Serial.begin(115200);
    network_manager.Begin(mac);
    Wire.begin(21, 22);
    pinMode(ADCpin,INPUT);
    Imu.begin();

    if(!LittleFS.begin(true))
    {
        Serial.println("LittleFS no montó. Formateando...");
        LittleFS.format();
        LittleFS.begin(true);
    }

    loadValues(doc);
    smp.zThreshold[0] = doc["thresholds"][0];
    smp.zThreshold[1] = doc["thresholds"][1];
    smp.yThreshold[0] = doc["thresholds"][2];
    smp.yThreshold[1] = doc["thresholds"][3];

    noneTimer = doc["timers"][0];
    forwardTimer = doc["timers"][1];
    backwardTimer = doc["timers"][2];   
   
    ssid = doc["network"]["ssid"] | "";
    password = doc["nwtwork"]["password"] | "";

    ModeManager::getInstance().setTimers(forwardTimer,backwardTimer);
    /*
    String voltage = "V:" + String(GetBatery());
    esp_cmd_t pkt = network_manager.CreatePacket(const_cast<char*>(voltage.c_str()), DATA);
    network_manager.Send(&pkt, 200);
    */
    Serial.println("X Y Z");
}

void PlotYPR()
{

    String xyz[3] = {"x","y", "z"};
    debug_msgs.plot(xyz,Imu.acc[0],Imu.acc[1],Imu.acc[2]);
    delay(10);
}
bool flagWifi = false;
void loop() {
    wifi_mode_t currentWiFiMode = WiFi.getMode();
    Imu.GetMotion();
    String direccion = smp.GetMov();
    
    if(GetBatery() <= 3.3)
    {
        debug_msgs.msg(debug_msgs.WARN, "Batería baja!");
        esp_deep_sleep_start();
    }

    if(!ModeManager::getInstance().selected)
    {
        ModeManager::getInstance().setMode();
        return;
    }

    if((ModeManager::getInstance().currentMode == ModeManager::MOVE))
    {
        if((direccion == "NONE") && (Imu.timer(noneTimer)))
        {
            ModeManager::getInstance().currentMode = ModeManager::NONE;
            ModeManager::getInstance().selected = false;
        }
    }

    if(!network_manager.espnowEnable)
        network_manager.dnsServer.processNextRequest();

    if(ModeManager::getInstance().currentMode == ModeManager::CONFIG && !flagWifi)
    {
        network_manager.serverInit(ssid,password);
        flagWifi = true;
    }

    delay(10);
}


//Mejorar sistema de calibración
//agregar alarma de deteccion de caidas
//imu como clase estatica con sigleton