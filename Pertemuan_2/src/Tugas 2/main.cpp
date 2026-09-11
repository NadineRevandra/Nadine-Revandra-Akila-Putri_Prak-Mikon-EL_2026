#include <Arduino.h>

const int PIN_BUTTON = 4;
const int PIN_LED_MERAH = 18;
const int PIN_LED_KUNING = 19;
const int PIN_LED_HIJAU = 21;

void setup() {
  // konfigurasi output LED
  pinMode(PIN_LED_MERAH, OUTPUT);
  pinMode(PIN_LED_KUNING, OUTPUT);
  pinMode(PIN_LED_HIJAU, OUTPUT);

  // konfigurasi Pin Button sebagai INPUT dengan internal Pull-Down
  pinMode(PIN_BUTTON, INPUT_PULLDOWN);
}

void loop() {
  // untuk membaca status Push Button
  int buttonState = digitalRead(PIN_BUTTON);

  if (buttonState == HIGH) {
    digitalWrite(PIN_LED_MERAH, HIGH);   // nyalakan merah
    digitalWrite(PIN_LED_KUNING, HIGH);  // nyalakan kuning
    digitalWrite(PIN_LED_HIJAU, HIGH);   // nyalakan hijau
  } 
  else {
    digitalWrite(PIN_LED_MERAH, LOW);    // matikan merah
    digitalWrite(PIN_LED_KUNING, LOW);   // matikan kuning
    digitalWrite(PIN_LED_HIJAU, LOW);    // matikan hijau
  }
}

