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

    digitalWrite(PWMA,HIGH);
    digitalWrite(PWMB,HIGH);
    digitalWrite(STBY,HIGH);
    digitalWrite(23,LOW);
}

Pins* Pins::instance = nullptr;
