#ifndef GESTURE_HANDLLER_H
#define GESTURE_HANDLLER_H

#include "IMU.h"
#include "Arduino.h"
#include "NetworkManager.h"
#include "DebugMsg.h"

class GestureHandler
{
    public:
        GestureHandler() : debug_msgs("GestureHandler.h") {}
        void GestureTest();
        float xThreshold[2];
        float yThreshold[2];
        float zThreshold[2];
        float hysteresis = 0.1;
    private:
        DebugMsgs debug_msgs;
        
};

#endif