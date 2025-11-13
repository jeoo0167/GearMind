#include "SMP.h"
#include "NetworkManager.h"
#include "Sounds.h"

auto& Network_manager = NetworkManager::getInstance();
extern Sounds buzzer;

String SMP::lastDirection = "NONE";
String SMP::currentDirection = "NONE";
unsigned long SMP::lastChangeTime = 0;

void SMP::GetMov()
{
    if (IMU::acc[2] >= (zThreshold[0] + hysteresis))
        currentDirection = "Forward";
    else if (IMU::acc[2] <= -(zThreshold[1] + hysteresis))
        currentDirection = "Backward";
    else if (IMU::acc[1] >= (yThreshold[0] + hysteresis))
        currentDirection = "Left";
    else if (IMU::acc[1] <= -(yThreshold[1] + hysteresis))
        currentDirection = "Right";
    else
        currentDirection = "NONE";

    unsigned long now = millis();

    if (currentDirection != lastDirection)
    {
        if (now - lastChangeTime > debounceTime)
        {
            lastDirection = currentDirection;
            lastChangeTime = now;

            debug_msgs.msg(debug_msgs.INFO, currentDirection.c_str());
            Network_manager.Send(currentDirection.c_str(),debounceTime);
            if(currentDirection != "NONE") buzzer.play_Sound1();
        }
    }
    else
    {
        lastChangeTime = now;
    }
}

