#include <Arduino.h>

const int PIN_BUTTON = 4;
const int PIN_LED_MERAH = 18;
const int PIN_LED_KUNING = 19;
const int PIN_LED_HIJAU = 21;

void setup() {
  // Konfigurasi Pin LED sebagai OUTPUT
  pinMode(PIN_LED_MERAH, OUTPUT);
  pinMode(PIN_LED_KUNING, OUTPUT);
  pinMode(PIN_LED_HIJAU, OUTPUT);

  // Konfigurasi Pin Button sebagai INPUT dengan internal Pull-Down
  pinMode(PIN_BUTTON, INPUT_PULLDOWN);
}

void loop() {
  // Membaca status Push Button
  int buttonState = digitalRead(PIN_BUTTON);

  if (buttonState == HIGH) {
    digitalWrite(PIN_LED_MERAH, HIGH);   // Nyalakan Merah
    digitalWrite(PIN_LED_KUNING, HIGH);  // Nyalakan Kuning
    digitalWrite(PIN_LED_HIJAU, HIGH);   // Nyalakan Hijau
  } 
  else {
    digitalWrite(PIN_LED_MERAH, LOW);    // Matikan Merah
    digitalWrite(PIN_LED_KUNING, LOW);   // Matikan Kuning
    digitalWrite(PIN_LED_HIJAU, LOW);    // Matikan Hijau
  }
}

