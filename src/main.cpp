#include "IMU.h"
#include "SMP.h"
#include "NetworkManager.h"
#include "DebugMsg.h"
#include "Sounds.h"
#include "String"

IMU Imu;
SMP smp;
Sounds buzzer;

DebugMsgs debug_msgs("main.cpp");

auto& network_manager = NetworkManager::getInstance(); 
uint8_t mac[6] = {0xF8, 0xB3, 0xB7, 0x20, 0x31, 0x18};

const int ADCpin = 36;


float GetBatery()
{
    int raw = analogRead(ADCpin);
    float v = (raw / 4095.0) * 3.3 *2;
    return v;
}

void setup() {
    buzzer.begin(17);
    Serial.begin(115200);
    network_manager.Begin(mac);
    Wire.begin(21, 22);
    pinMode(ADCpin,INPUT);
    Imu.begin();

    smp.zThreshold[0] = 1.2;
    smp.zThreshold[1] = -0.88;
    smp.yThreshold[0] = 0.30;
    smp.yThreshold[1] = 0.40;

    String voltage = "V:" + String(GetBatery());
    esp_cmd_t pkt = network_manager.CreatePacket(const_cast<char*>(voltage.c_str()), DATA);
    network_manager.Send(&pkt, 200);

    Serial.println("X Y Z");
}

void PlotYPR()
{

    String xyz[3] = {"x","y", "z"};
    debug_msgs.plot(xyz,Imu.acc[0],Imu.acc[1],Imu.acc[2]);
    delay(10);
}

void loop() {
    if(GetBatery() <= 3.3)
    {
        debug_msgs.msg(debug_msgs.WARN, "Batería baja!");
        esp_deep_sleep_start();
    }
    Imu.GetMotion();
    
    smp.GetMov();
    //PlotYPR();
    delay(10);
}


// -Optimizar codigo:
//      -funcion create packet en network manager.h
//Mejoras:
// -añadir sonidos a robot driver
// -modulacion pwm
// -sistema de gestos
// -App web
// -sonidos