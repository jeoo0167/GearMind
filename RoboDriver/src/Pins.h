#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

class Pins 
{
    private:
        static Pins* instance;
        Pins() {}
    public: 

        static Pins* getInstance()
        {  
            if(!instance)
                instance = new Pins();
            return instance;
        }

        void init();
        
        int PWMA = 14;
        int PWMB = 12;
        int IN1 = 4;
        int IN2 = 5;
        int IN3 = 18;
        int IN4 = 19;
        int STBY = 15;

};

#endif