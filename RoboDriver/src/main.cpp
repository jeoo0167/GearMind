#include <Arduino.h>
#include "Pins.h"
#include "Motors.h"
#include "NetworkManager.h"

Motors motors;
auto* pins_ = Pins::getInstance();

void setup()
{
  Serial.begin(115200);
  ledcSetup(1, 2000, 8);
  ledcSetup(2, 2000, 8);
  ledcAttachPin(pins_->PWMA, 1);
  ledcAttachPin(pins_->PWMB, 2);
  pins_->init();
  NetworkManager::getInstance().Begin();
  motors.setSpeed(150);
}

void loop() 
{
}