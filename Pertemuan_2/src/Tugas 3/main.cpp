#include <Arduino.h>

const int potPin = 34;        // pin analog (ADC) untuk membaca wiper potensiometer
const float VREF   = 3.3;     
const int   ADC_MAX = 4095;   
const float R_TOTAL = 10000.0; // nilai total resistansi potensiometer (10k Ohm)

void setup() {
  Serial.begin(115200);       
}

void loop() {
  int rawADC = analogRead(potPin);               

  float tegangan = (rawADC / (float)ADC_MAX) * VREF;   // konversi ke Volt
  float hambatan = (tegangan / VREF) * R_TOTAL;        // konversi ke Ohm (posisi wiper)

  Serial.print("Raw ADC: ");
  Serial.print(rawADC);
  Serial.print("\tTegangan: ");
  Serial.print(tegangan, 3);
  Serial.print(" V\tHambatan: ");
  Serial.print(hambatan, 1);
  Serial.println(" Ohm");

  delay(500);   // untuk jeda pembacaan tiap siklus
}