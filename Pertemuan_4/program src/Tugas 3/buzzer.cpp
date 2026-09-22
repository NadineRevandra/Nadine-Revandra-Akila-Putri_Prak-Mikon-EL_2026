#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>

#define BUZZER_PIN 26

TwoWire WireOLED = TwoWire(1);

Adafruit_MPU6050 mpu;
Adafruit_SSD1306 display(128, 64, &WireOLED, -1); 

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
 
  Wire.begin(21, 22);      
  WireOLED.begin(32, 33); 

  mpu.begin(0x68, &Wire);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  display.clearDisplay();
  
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("X: ");
  display.print(a.acceleration.x);
  display.println(" m/s^2");

  display.setCursor(0, 16);
  display.print("Y: ");
  display.print(a.acceleration.y);
  display.println(" m/s^2");

  // jika posisi sensor miring (sumbu X atau Y melebihi batas 3.0 m/s^2)
  if (abs(a.acceleration.x) > 3.0 || abs(a.acceleration.y) > 3.0) {
    display.setCursor(0, 36);
    display.setTextSize(2);
    display.print("MIRING");
    digitalWrite(BUZZER_PIN, HIGH); // buzzer Menyala
  } else {
    display.setCursor(0, 36);
    display.setTextSize(2);
    display.print("DATAR");
    digitalWrite(BUZZER_PIN, LOW);  // buzzer Mati
  }

  display.display();
  delay(100);
}