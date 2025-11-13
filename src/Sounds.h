#ifndef SOUNDS_H
#define SOUNDS_H

#include <Arduino.h>

class Sounds
{
    public:
        void begin(int pin, int channel = 1, int resolution = 8, int baseFreq = 2000);
        void play_Sound1();
        void play_Sound2();
        void play_Sound3();
    private:
        int _pin;
        int _channel;
};


#endif