#include "Motors.h"

void Motors::Forward()
{
    digitalWrite(Pins::getInstance()->IN1,HIGH);
    digitalWrite(Pins::getInstance()->IN2,LOW);
    digitalWrite(Pins::getInstance()->IN3,HIGH);
    digitalWrite(Pins::getInstance()->IN4,LOW);
}

void Motors::Backward()
{
    digitalWrite(Pins::getInstance()->IN1,LOW);
    digitalWrite(Pins::getInstance()->IN2,HIGH);
    digitalWrite(Pins::getInstance()->IN3,LOW);
    digitalWrite(Pins::getInstance()->IN4,HIGH);
}

void Motors::Left()
{
    digitalWrite(Pins::getInstance()->IN1,LOW);
    digitalWrite(Pins::getInstance()->IN2,HIGH);
    digitalWrite(Pins::getInstance()->IN3,HIGH);
    digitalWrite(Pins::getInstance()->IN4,LOW);
}

void Motors::Right()
{
    digitalWrite(Pins::getInstance()->IN1,HIGH);
    digitalWrite(Pins::getInstance()->IN2,LOW);
    digitalWrite(Pins::getInstance()->IN3,LOW);
    digitalWrite(Pins::getInstance()->IN4,HIGH);
}

void Motors::Stop()
{
    digitalWrite(Pins::getInstance()->IN1,LOW);
    digitalWrite(Pins::getInstance()->IN2,LOW);
    digitalWrite(Pins::getInstance()->IN3,LOW);
    digitalWrite(Pins::getInstance()->IN4,LOW);
}