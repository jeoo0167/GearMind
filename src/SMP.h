#ifndef SMP_H
#define SMP_H

#include "IMU.h"
#include "Arduino.h"
#include "NetworkManager.h"
#include "DebugMsg.h"

class SMP
{
    public:
        SMP() : debug_msgs("SMP.h") {}
        void GetMov();
        float xThreshold[2];
        float yThreshold[2];
        float zThreshold[2];
    private:
        DebugMsgs debug_msgs;
        const unsigned long debounceTime = 200;
        const float hysteresis = 0.05;           

        static String lastDirection;
        static unsigned long lastChangeTime;
        static String currentDirection;
        
};

#endif