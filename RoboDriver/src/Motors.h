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
};

#endif