#include "IMU.h"
#include "SMP.h"
#include "NetworkManager.h"
#include "DebugMsg.h"
#include "Sounds.h"
#include "String"

IMU Imu;//change
SMP smp;
Sounds buzzer;

DebugMsgs debug_msgs("main.cpp");

auto& network_manager = NetworkManager::getInstance(); 
uint8_t mac[6] = {0xF8, 0xB3, 0xB7, 0x20, 0x31, 0x18};

const int ADCpin = 36;


float GetBatery()
{
    int raw = analogRead(ADCpin);
    float v = raw / 4095.0 * 3.3;
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
    debug_msgs.msg(debug_msgs.INFO,"Battery %.2f:",GetBatery());

    Serial.println("X Y Z");
}

void PlotYPR()
{

    String xyz[3] = {"x","y", "z"};
    debug_msgs.plot(xyz,Imu.acc[0],Imu.acc[1],Imu.acc[2]);
    delay(10);
}

void loop() {
    Imu.GetMotion();
    
    smp.GetMov();
    //PlotYPR();
    delay(10);
}


