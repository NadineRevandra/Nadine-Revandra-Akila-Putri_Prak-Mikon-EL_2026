#include <Arduino.h>

const int potPin = 18; // potensiometer pwm input
const int servoPin = 16;
const int freq = 50;
const int channel = 0;
const int res = 12;

void setup() {
  ledcSetup(channel, freq, res);
  ledcAttachPin(servoPin, channel);
}

void loop() {
  int potValue = analogRead(potPin); // Hasil: 0 - 4095 pwm pwm
  int pwmValue = map(potValue, 0, 4095, 0, 512); // Konversi ke 0 - 255 pwm 
  
  
  ledcWrite(channel, pwmValue);
  delay(15);
}

