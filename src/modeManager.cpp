#include "modeManager.h"

extern SMP smp;
extern Sounds buzzer;
extern IMU Imu;

bool ModeManager::selected = false;
ModeManager::Modes ModeManager::currentMode = ModeManager::NONE;

void ModeManager::setTimers(unsigned long Tf,unsigned long Tb)
{
    Time_forward = Tf;
    Time_backward = Tb;
}

void ModeManager::setMode()
{
    current_dir = smp.GetMov();
    if(current_dir == "NONE")
    {
        Imu.timer(0);
        return;
    }

    if(current_dir == "Forward")
    {
        if(Imu.timer(Time_forward))
        {
            currentMode = MOVE;
            selected = true;
            Serial.println("Mode: MOVE");
            buzzer.play_Sound1();
        }
        return;
    }

    if(current_dir == "Backward")
    {
        if(Imu.timer(Time_backward))
        {
            selected = true;
            currentMode = CONFIG;
            Serial.println("Mode: CONFIG");
            buzzer.play_Sound1();
        }
        return;
    }
}
