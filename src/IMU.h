#ifndef IMU_H
#define IMU_H

#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>
#include "DebugMsg.h"   // para logs

class IMU {
public:
    IMU() : mpu(), calib_rounds(500), lst_time(0) {}

    void begin();          // Inicializa el sensor y calibra
    void GetMotion();      // Lee aceleración y giroscopio (ya calibrado)
    
    static float ToRadian(float grade);
    static float ToGrade(float radian);

    // Datos públicos (en unidades físicas)
    static float acc[3];          // [g]
    static float gyro[3];         // [°/s]

    static float Gyro_sens;   // LSB/(°/s)
    static float Acc_sens;    // LSB/g

private:
    MPU6050 mpu;
    DebugMsgs debug_msgs = DebugMsgs("IMU.cpp");

    // Calibración
    const int calib_rounds;   // número de muestras promedio
    float accOffset[3];
    float gyroOffset[3];

    unsigned long lst_time;
};

#endif
