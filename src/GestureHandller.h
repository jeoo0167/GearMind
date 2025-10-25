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
        int axisThresholds[3] = {0,0,0};  

    private:
        DebugMsgs debug_msgs;
        bool getNone = false;
        bool getForward = false;
        bool getBackward = false;
        bool getRight = false;
        bool getLeft = false;
        

};

#endif