#include <Arduino.h>

const int servoPin = 16;
const int freq = 50;
const int channel = 0;
const int res = 12;

void setup() {
  ledcSetup(channel, freq, res);
  ledcAttachPin(servoPin, channel);
}

void loop() {
  ledcWrite(channel, 102); // Posisi ~0 derajat
  delay(1000);
  ledcWrite(channel, 512); // Posisi ~180 derajat
  delay(1000);
}


