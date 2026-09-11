#include <Arduino.h>

const int potPin    = 34;
const int buttonPin = 27;
const int ledMerah  = 25;
const int ledKuning = 26;
const int ledHijau  = 33;

const int ADC_MAX = 4095;
// batas level tegangan, membagi rentang 0-4095 menjadi 3 bagian sama rata
const int BATAS_RENDAH = ADC_MAX / 3;        // ~1365
const int BATAS_SEDANG = (ADC_MAX * 2) / 3;  // ~2730

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);  // Default HIGH, jika button ditekan maka LOW 
  pinMode(ledMerah, OUTPUT);
  pinMode(ledKuning, OUTPUT);
  pinMode(ledHijau, OUTPUT);
}

void matikanSemuaLED() {
  digitalWrite(ledMerah, LOW);
  digitalWrite(ledKuning, LOW);
  digitalWrite(ledHijau, LOW);
}

void loop() {
  int rawADC = analogRead(potPin);
  bool tombolDarurat = (digitalRead(buttonPin) == LOW);  // LOW = tombol ditekan (pulled-up)

  if (tombolDarurat) {
    // Tombol darurat ditekan -> seluruh LED WAJIB mati, abaikan nilai potensiometer
    matikanSemuaLED();
    Serial.println("TOMBOL DARURAT DITEKAN - Semua LED MATI");
  } else {
    if (rawADC < BATAS_RENDAH) {
      // Level Tegangan Rendah -> hanya LED Merah menyala
      digitalWrite(ledMerah, HIGH);
      digitalWrite(ledKuning, LOW);
      digitalWrite(ledHijau, LOW);
      Serial.print("Level RENDAH | Raw ADC: ");
      Serial.println(rawADC);
    } else if (rawADC < BATAS_SEDANG) {
      // Level Tegangan Sedang -> hanya LED Kuning menyala
      digitalWrite(ledMerah, LOW);
      digitalWrite(ledKuning, HIGH);
      digitalWrite(ledHijau, LOW);
      Serial.print("Level SEDANG | Raw ADC: ");
      Serial.println(rawADC);
    } else {
      // Level Tegangan Tinggi -> hanya LED Hijau menyala
      digitalWrite(ledMerah, LOW);
      digitalWrite(ledKuning, LOW);
      digitalWrite(ledHijau, HIGH);
      Serial.print("Level TINGGI | Raw ADC: ");
      Serial.println(rawADC);
    }
  }

  delay(100);  // Delay pembacaan 100 ms sesuai instruksi
}