#include "IMU.h"


float IMU::ToRadian(float grade) {return (grade*M_PI)/180;}
float IMU::ToGrade(float radian) {return (radian*180)/M_PI;}

void IMU::begin()
{
    mpu.initialize();

    debug_msgs.msg(debug_msgs.INFO,"Testing connection . . .");
    mpu.testConnection() ? debug_msgs.msg(debug_msgs.INFO,"Connection ok") : 
    debug_msgs.msg(debug_msgs.ERROR,"Connection wrong");

    debug_msgs.msg(debug_msgs.INFO,"Starting dmp . . .");
    uint8_t devSatus = mpu.dmpInitialize();

    delay(50);
    

    if (devSatus == 0) {
        debug_msgs.msg(debug_msgs.INFO,"Calibrating using library functions...");
        mpu.CalibrateAccel(15);
        mpu.CalibrateGyro(15);
        mpu.PrintActiveOffsets();
        

    mpu.setDMPEnabled(true);
    dmpReady = true;
    }
    else
    {
        debug_msgs.msg(debug_msgs.ERROR,"Falied to start dmp");
        Serial.println(devSatus);
    }

    mpu.resetFIFO();
    delay(100);
}


void IMU::GetYPR(float *ypr, int n)
{
    if (!dmpReady) return;  
    
    uint16_t packetSize = mpu.dmpGetFIFOPacketSize();
    uint16_t fifoCount = mpu.getFIFOCount();

    if (fifoCount < packetSize) return;

    if (fifoCount > 1024) {
        mpu.resetFIFO();
        debug_msgs.msg(debug_msgs.WARN, "FIFO overflow !");
        return;
    }

    if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

        // Convertir de radianes a grados
        for (int i = 0; i < n; i++) {
            ypr[i] = ToGrade(ypr[i]);
        }
    }

}
