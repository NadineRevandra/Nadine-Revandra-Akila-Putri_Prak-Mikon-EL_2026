#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  mpu.begin();
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Serial.print("Sumbu X: ");
  Serial.println(a.acceleration.x);
  Serial.print("Sumbu Y: ");
  Serial.println(a.acceleration.y);
  Serial.print("Sumbu Z: ");
  Serial.println(a.acceleration.z);
  
  delay(500);
}