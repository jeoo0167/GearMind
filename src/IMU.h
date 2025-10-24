#ifndef IMU_H
#define IMU_H

#include <Arduino.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "DebugMsg.h"

class IMU 
{
    public:
        void begin();
        static const int SDA = 22;
        static const int SCL = 21;        
        MPU6050 mpu;
        Quaternion q;
        VectorFloat gravity;
        volatile bool dmpReady = false;
        volatile bool imuInterrupt = false;
        
        uint8_t fifoBuffer[64];
            
        static float ypr[3];
        float ToGrade(float grade);
        float ToRadian(float grade);
        bool Calibrate(int samples);
        void GetYPR(float *ypr, int n=3);
        IMU() : debug_msgs("IMU.cpp") {}
    private:
        DebugMsgs debug_msgs;

        float pitchFiltered = 0;
        float alpha = 2.5;
};

#endif