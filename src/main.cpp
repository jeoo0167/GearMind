#include "IMU.h"
#include "GestureHandller.h"
#include "NetworkManager.h"
#include "DebugMsg.h"

IMU Imu;
GestureHandler Gh;

DebugMsgs debug_msgs("main.cpp");

auto& network_manager = NetworkManager::getInstance(); 
uint8_t mac[6] = {0xF8, 0xB3, 0xB7, 0x20, 0x31, 0x18};

float IMU::ypr[3] = {0.0f, 0.0f, 0.0f};   // definición

void setup() {
    Serial.begin(115200);
    network_manager.Begin(mac);
    Wire.begin(21, 22);
    Imu.begin();

    Gh.axisThresholds[0] = 25;
    Gh.axisThresholds[1] = 10;
    Serial.println("X Y Z"); 
}

void PlotYPR()
{
   // debug_msgs.msg(DebugMsgs::INFO,
   //             "Yaw: %.2f  Pitch: %.2f  Roll: %.2f",
   //             Imu.ypr[0], Imu.ypr[1], Imu.ypr[2]);
  Serial.print(">");

  Serial.print("X:");
  Serial.print(Imu.ypr[0]);
  Serial.print(",");
  Serial.print("Y:");
  Serial.print(Imu.ypr[1]);
  Serial.print(",");
  Serial.print("Z:");
  Serial.print(Imu.ypr[2]);
  Serial.println();
   delay(50);
}

void loop() {
    Imu.GetYPR(Imu.ypr);
    
    Gh.GestureTest();
    //PlotYPR();
}
