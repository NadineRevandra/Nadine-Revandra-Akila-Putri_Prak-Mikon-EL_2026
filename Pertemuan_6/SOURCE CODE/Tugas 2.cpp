#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>

const char* ssid = "tum";
const char* password = "nadinerev";

WebServer server(80);
Adafruit_MPU6050 mpu; // SDA ke GPIO 21, SCL ke GPIO 22

unsigned long waktuSebelumnya = 0;
const long interval = 2000;

// Membaca sudut kemiringan (Roll) dalam derajat (0 - 180 derajat)
float bacaKemiringan(float &nilaiAccelY) {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  nilaiAccelY = a.acceleration.y;
  float roll = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI; // (1) percepatan sumbu Y lalu sumbu Z[cite: 4, 10]
  return abs(roll); // (2) nilai mutlak dari roll[cite: 4, 10]
}

String tentukanStatus(float sudut) {
  if (sudut > 35.0) return "BAHAYA"; // (3) batas sudut status BAHAYA: 35 derajat[cite: 4, 10]
  else if (sudut >= 15.0) return "SEDANG"; // (4) batas sudut status SEDANG: 15 derajat[cite: 4, 10]
  else return "DATAR";
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Tugas 2 - Sensor Kemiringan MPU6050</title>
<style>
body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }
.badge { padding: 15px 30px; color: white; font-weight: bold; border-radius: 8px; display: inline-block; font-size: 24px; }
.DATAR { background-color: #6c757d; }
.SEDANG { background-color: #28a745; }
.BAHAYA { background-color: #dc3545; }
</style>
</head>
<body>
<h2>Pemantauan Sudut Kemiringan (MPU6050)</h2>
<h3>Sudut Kemiringan: <span id="sudut">--</span> &deg; (Accel Y: <span id="accY">--</span> m/s&sup2;)</h3>
<div id="statusBox" class="badge SEDANG">MEMUAT...</div>
<script>
function ambilData() {
  fetch("/data")
  .then(res => res.json())
  .then(data => {
    document.getElementById("sudut").innerText = data.sudut.toFixed(1);
    document.getElementById("accY").innerText = data.accY.toFixed(2);
    let box = document.getElementById("statusBox");
    box.innerText = data.status;
    box.className = "badge " + data.status;
  });
}

setInterval(ambilData, 2000);
ambilData();
</script>
</body>
</html>
)rawliteral";

void handleData() {
  float accY = 0;
  float sudut = bacaKemiringan(accY);
  String status = tentukanStatus(sudut);
  String json = "{\"sudut\":" + String(sudut) + ",\"accY\":" + String(accY) + ",\"status\":\"" + status + "\"}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);

  if (!mpu.begin()) { // (5) fungsi untuk memulai sensor MPU6050[cite: 4, 11]
    Serial.println("Gagal menemukan sensor MPU6050! Cek kabel SDA(21) & SCL(22).");
    while (1) { delay(10); }
  }
  Serial.println("MPU6050 Siap!");

  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nTerhubung! IP Address: " + WiFi.localIP().toString());

  server.on("/", []() { server.send(200, "text/html", index_html); });
  server.on("/data", handleData); // (6) path yang dipanggil fetch() di halaman web[cite: 4, 11]
  server.begin();
}

void loop() {
  server.handleClient();
  unsigned long waktuSekarang = millis();
  if (waktuSekarang - waktuSebelumnya >= interval) {
    waktuSebelumnya = waktuSekarang;
    float accY = 0;
    float sudut = bacaKemiringan(accY);
    String status = tentukanStatus(sudut);
    Serial.print("Accel Y: ");
    Serial.print(accY, 2);
    Serial.print(" m/s2 | Sudut Miring: ");
    Serial.print(sudut, 1);
    Serial.print("° | Status: ");
    Serial.println(status);
  }
}