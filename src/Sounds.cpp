#include <Sounds.h>

extern IMU imu;

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

void Sounds::play_Sound2()
{
  for (int x =0;x<=35;x++)
  {
    float exp_tone = 100 + pow(x,2);
    ledcWriteTone(_channel,exp_tone);
    delay(25);
  }
  ledcWriteTone(_channel,0);
}