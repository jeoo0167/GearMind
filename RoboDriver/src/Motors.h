#ifndef Motors_H
#define Motors_H

#include "Pins.h"

class Motors
{
    public:
        void Forward();
        void Backward();
        void Left();
        void Right();
        void Stop();
        void setSpeed(int speed);

        float linearSpeed = 178.0;
        float angularSpeed = 115.0;
};

#endif