#ifndef IMU_H
#define IMU_H

#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>
#include "DebugMsg.h"
#include "Sounds.h"

class IMU {
public:
    IMU() : mpu(), calib_rounds(500), lst_time(0) {}

    void begin();
    void GetMotion();
    
    static float ToRadian(float grade);
    static float ToGrade(float radian);

    static float acc[3];
    static float gyro[3];  

    static float Gyro_sens;
    static float Acc_sens;

private:
    MPU6050 mpu;
    DebugMsgs debug_msgs = DebugMsgs("IMU.cpp");

    const int calib_rounds;
    float accOffset[3];
    float gyroOffset[3];

    unsigned long lst_time;
};

#endif
