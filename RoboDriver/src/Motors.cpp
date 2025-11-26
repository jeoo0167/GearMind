#include "Motors.h"

extern Pins* pins_;

void Motors::Forward()
{
    setSpeed(linearSpeed);
    digitalWrite(pins_->IN1,HIGH);
    digitalWrite(pins_->IN2,LOW);
    digitalWrite(pins_->IN3,HIGH);
    digitalWrite(pins_->IN4,LOW);
}

void Motors::Backward()
{
    setSpeed(linearSpeed);
    digitalWrite(pins_->IN1,LOW);
    digitalWrite(pins_->IN2,HIGH);
    digitalWrite(pins_->IN3,LOW);
    digitalWrite(pins_->IN4,HIGH);
}

void Motors::Left()
{
    setSpeed(angularSpeed);
    digitalWrite(pins_->IN1,LOW);
    digitalWrite(pins_->IN2,HIGH);
    digitalWrite(pins_->IN3,HIGH);
    digitalWrite(pins_->IN4,LOW);
}

void Motors::Right()
{
    setSpeed(angularSpeed);
    digitalWrite(pins_->IN1,HIGH);
    digitalWrite(pins_->IN2,LOW);
    digitalWrite(pins_->IN3,LOW);
    digitalWrite(pins_->IN4,HIGH);
}

void Motors::Stop()
{
    digitalWrite(pins_->IN1,LOW);
    digitalWrite(pins_->IN2,LOW);
    digitalWrite(pins_->IN3,LOW);
    digitalWrite(pins_->IN4,LOW);
}

void Motors::setSpeed(int speed)
{
    ledcWrite(pins_->ch1, speed);
    ledcWrite(pins_->ch2, speed);
}