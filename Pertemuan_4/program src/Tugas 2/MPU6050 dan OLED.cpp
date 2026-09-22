#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>

TwoWire WireOLED = TwoWire(1);
Adafruit_MPU6050 mpu;
Adafruit_SSD1306 display(128, 64, &WireOLED, -1); 

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);      // MPU6050 di Pin 21 & 22
  WireOLED.begin(32, 33);  // OLED di Pin 32 & 33

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
  
  display.setCursor(0, 0);
  display.print("X: ");
  display.print(a.acceleration.x);
  display.println(" m/s^2");

  display.setCursor(0, 20);
  display.print("Y: ");
  display.print(a.acceleration.y);
  display.println(" m/s^2");

  display.setCursor(0, 40);
  display.print("Z: ");
  display.print(a.acceleration.z);
  display.println(" m/s^2");

  display.display();
  delay(200); // ubah ke 200ms agar respon di layar lebih smooth
}