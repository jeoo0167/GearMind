#include "IMU.h"
#include "GestureHandller.h"
#include "NetworkManager.h"
#include "DebugMsg.h"

IMU Imu;
GestureHandler Gh;

DebugMsgs debug_msgs("main.cpp");

auto& network_manager = NetworkManager::getInstance(); 
uint8_t mac[6] = {0xF8, 0xB3, 0xB7, 0x20, 0x31, 0x18};

void setup() {
    Serial.begin(115200);
    network_manager.Begin(mac);
    Wire.begin(21, 22);
    Imu.begin();

    Gh.zThreshold[0] = 1.2;
    Gh.zThreshold[1] = -0.88;
    Gh.yThreshold[0] = 0.30;
    Gh.yThreshold[1] = 0.40;
    Serial.println("X Y Z"); 
}

void PlotYPR()
{
   // debug_msgs.msg(DebugMsgs::INFO,
   //             "Yaw: %.2f  Pitch: %.2f  Roll: %.2f",
   //             Imu.ypr[0], Imu.ypr[1], Imu.ypr[2]);
    String xyz[3] = {"x","y", "z"};
    debug_msgs.plot(xyz,Imu.acc[0],Imu.acc[1],Imu.acc[2]);
    delay(10);
}

void loop() {
    Imu.GetMotion();
    
    Gh.GestureTest();
    //PlotYPR();
    delay(10);
}
