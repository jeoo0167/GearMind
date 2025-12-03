#ifndef MODE_MANAGER_H
#define MODE_MANAGER_H

#include "SMP.h"
#include "Sounds.h"
#include "String.h"
#include "IMU.h"

class ModeManager
{
public:
    enum Modes 
    {
        NONE,
        MOVE,
        CONFIG,
        DEBUG
    };

    static ModeManager& getInstance()
    {
        static ModeManager instance;
        return instance;
    }

    ModeManager(const ModeManager&) = delete;
    ModeManager& operator=(const ModeManager&) = delete;
    ModeManager() = default;

    static Modes currentMode;
    static bool selected;

    void setMode();
    void setTimers(unsigned long Tf,unsigned long Tb);

private:
    String current_dir;
    unsigned long Time_forward;
    unsigned long Time_backward;
    
    
};

#endif
