#include <Arduino.h>
#include "Pins.h"
#include "Motors.h"
#include "NetworkManager.h"

Motors motors;

void setup()
{
  Serial.begin(115200);
  Pins::getInstance()->init();
  NetworkManager::getInstance().Begin();
}

void loop() 
{
}