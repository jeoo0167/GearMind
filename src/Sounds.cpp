#include <Sounds.h>


void Sounds::begin(int pin, int channel, int resolution, int baseFreq) {
    _pin = pin;
    _channel = channel;
    ledcSetup(_channel, baseFreq, resolution);
    ledcAttachPin(_pin, _channel);
    Serial.println("Attached");
}

void Sounds::play_Sound1()
{
  ledcWriteTone(_channel, 1000);
  delay(500);
  ledcWriteTone(_channel, 0);    
  delay(500);
}