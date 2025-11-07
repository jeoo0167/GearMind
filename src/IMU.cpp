#include "IMU.h"

float IMU::Gyro_sens = 131.0;
float IMU::Acc_sens = 16384.0; // ±2g
float IMU::gyro[3] = {0};
float IMU::acc[3] = {0};
float IMU::ToRadian(float grade) { return (grade * M_PI) / 180.0; }
float IMU::ToGrade(float radian) { return (radian * 180.0) / M_PI; }

void IMU::begin()
{
    mpu.initialize();

    debug_msgs.msg(debug_msgs.INFO, "Testing connection . . .");
    if (mpu.testConnection())
        debug_msgs.msg(debug_msgs.INFO, "Connection OK");
    else
        debug_msgs.msg(debug_msgs.ERROR, "Connection FAILED");

    delay(100);

    debug_msgs.msg(debug_msgs.INFO, "Calibrating offsets (keep still)");

    long accSum[3] = {0}, gyroSum[3] = {0};
    for (int i = 0; i < calib_rounds; i++)
    {
        int16_t ax, ay, az, gx, gy, gz;
        mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

        accSum[0] += ax;
        accSum[1] += ay;
        accSum[2] += az;
        gyroSum[0] += gx;
        gyroSum[1] += gy;
        gyroSum[2] += gz;

        if (i % 100 == 0) Serial.print(".");
        delay(5);
    }

    for (int i = 0; i < 3; i++)
    {
        accOffset[i] = (float)accSum[i] / calib_rounds;
        gyroOffset[i] = (float)gyroSum[i] / calib_rounds;
    }

    // Ajustar el eje Z del acelerómetro para compensar la gravedad
    accOffset[2] -= Acc_sens;  // si el sensor está quieto con Z hacia arriba

    Serial.println("\nCalibration done!");
}

void IMU::GetMotion()
{
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    acc[0] = (ax - accOffset[0]) / Acc_sens;
    acc[1] = (ay - accOffset[1]) / Acc_sens;
    acc[2] = (az - accOffset[2]) / Acc_sens;

    gyro[0] = (gx - gyroOffset[0]) / Gyro_sens;
    gyro[1] = (gy - gyroOffset[1]) / Gyro_sens;
    gyro[2] = (gz - gyroOffset[2]) / Gyro_sens;
}
