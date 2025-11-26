#include "Pins.h"

void Pins::init()
{
    pinMode(STBY,OUTPUT);
    pinMode(PWMA,OUTPUT);
    pinMode(PWMB,OUTPUT);
    pinMode(IN1,OUTPUT);
    pinMode(IN2,OUTPUT);
    pinMode(IN3,OUTPUT);
    pinMode(IN4,OUTPUT);
    pinMode(23,OUTPUT);

    digitalWrite(STBY,HIGH);
    digitalWrite(23,LOW);

    ledcSetup(ch1, freq, resolution);
    ledcSetup(ch2, freq, resolution);

    ledcAttachPin(PWMA, ch1);
    ledcAttachPin(PWMB, ch2);
}

Pins* Pins::instance = nullptr;
