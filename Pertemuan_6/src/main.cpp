<<<<<<< HEAD
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>

const char* ssid = "tum";
const char* password = "nadinerev";

WebServer server(80);
Servo myServo;
Adafruit_MPU6050 mpu;

// Pin ESP32
const int servoPin = 18;
const int buzzerPin = 23;

// Variabel sistem
int currentServoPos = 90;
float accYTerakhir = 0.0;
float sudutTerakhir = 0.0;
String statusTerakhir = "";
String buzzerTerakhir = "OFF";

unsigned long waktuSebelumnya = 0;
const long interval = 2000;

// Memeriksa input angka
bool isNumeric(String str) {
  if (str.length() == 0) return false;

  for (unsigned int i = 0; i < str.length(); i++) {
    if (!isDigit(str.charAt(i))) return false;
  }

  return true;
}

// Membaca sudut kemiringan MPU6050
float bacaKemiringan(float &nilaiAccelY) {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  nilaiAccelY = a.acceleration.y;

  float roll = atan2(
    a.acceleration.y,
    a.acceleration.z
  ) * 180.0 / PI;

  return fabs(roll);
}

// Pemeriksaan kondisi sensor
void cekKondisiMPU() {
  sudutTerakhir = bacaKemiringan(accYTerakhir);

  String statusBaru = "";

  // Status BAHAYA
  if (sudutTerakhir > 35.0) {
    statusBaru = "BAHAYA";
    buzzerTerakhir = "ON (MENYALA)";

    digitalWrite(buzzerPin, HIGH);

    currentServoPos = 180;
    myServo.write(currentServoPos);
  }

  // Status SEDANG
  else if (sudutTerakhir >= 15.0) {
    statusBaru = "SEDANG";
    buzzerTerakhir = "OFF";

    digitalWrite(buzzerPin, LOW);

    // Servo bergerak hanya saat status berubah
    if (statusTerakhir != "SEDANG") {
      currentServoPos = 90;
      myServo.write(currentServoPos);
    }
  }

  // Status DATAR
  else {
    statusBaru = "DATAR";
    buzzerTerakhir = "OFF";

    digitalWrite(buzzerPin, LOW);

    // Servo bergerak hanya saat status berubah
    if (statusTerakhir != "DATAR") {
      currentServoPos = 0;
      myServo.write(currentServoPos);
    }
  }

  statusTerakhir = statusBaru;

  // Informasi di Serial Monitor
  Serial.print("Accel Y: ");
  Serial.print(accYTerakhir, 2);
  Serial.print(" m/s2 | Sudut: ");
  Serial.print(sudutTerakhir, 1);
  Serial.print(" derajat | Kondisi: ");
  Serial.print(statusTerakhir);
  Serial.print(" | Buzzer: ");
  Serial.print(buzzerTerakhir);
  Serial.print(" | Servo: ");
  Serial.print(currentServoPos);
  Serial.println(" derajat");
}

// Halaman web dashboard
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Tugas 3 - MPU6050 + Servo + Buzzer</title>

<style>
body {
  font-family: Arial, sans-serif;
  text-align: center;
  margin-top: 30px;
  padding: 0 10px;
}

.badge {
  padding: 12px 25px;
  color: white;
  font-weight: bold;
  border-radius: 8px;
  display: inline-block;
  font-size: 20px;
}

.DATAR {
  background-color: #6c757d;
}

.SEDANG {
  background-color: #28a745;
}

.BAHAYA {
  background-color: #dc3545;
}

.card {
  border: 1px solid #ccc;
  padding: 20px;
  margin: 15px auto;
  max-width: 340px;
  border-radius: 10px;
}

input[type=range] {
  width: 90%;
}
</style>
</head>

<body>
<h2>Sistem Otomatis Kemiringan
(MPU6050, Servo & Buzzer)</h2>

<div class="card">
  <h3>Status Sensor MPU6050</h3>

  <p>
    Kemiringan: <span id="sudut">--</span> &deg;
    (Accel Y: <span id="accY">--</span> m/s&sup2;)
  </p>

  <div id="statusBox" class="badge SEDANG">
    MEMUAT...
  </div>

  <p>Buzzer: <b id="buzzerStatus">OFF</b></p>
</div>

<div class="card">
  <h3>Kendali Servo</h3>

  <p>Sudut Servo: <span id="posVal">90</span>&deg;</p>

  <input
    type="range"
    min="0"
    max="180"
    value="90"
    id="servoSlider"
    oninput="updateServo(this.value)"
  >

  <p id="statusMsg"></p>
</div>

<script>
function updateServo(val) {
  document.getElementById("posVal").innerText = val;

  fetch("/servo?pos=" + val)
    .then(res =>
      res.text().then(text => ({
        status: res.status,
        body: text
      }))
    )
    .then(res => {
      document.getElementById("statusMsg").innerText =
        "Status " + res.status + ": " + res.body;
    })
    .catch(() => {
      document.getElementById("statusMsg").innerText =
        "Gagal menghubungi ESP32";
    });
}

function ambilData() {
  fetch("/data")
    .then(res => res.json())
    .then(data => {
      document.getElementById("sudut").innerText =
        data.sudut.toFixed(1);

      document.getElementById("accY").innerText =
        data.accY.toFixed(2);

      let box = document.getElementById("statusBox");
      box.innerText = data.status;
      box.className = "badge " + data.status;

      document.getElementById("buzzerStatus").innerText =
        data.buzzer;

      document.getElementById("posVal").innerText =
        data.servo;

      document.getElementById("servoSlider").value =
        data.servo;
    })
    .catch(err => console.log("Gagal mengambil data:", err));
}

setInterval(ambilData, 2000);
ambilData();
</script>

</body>
</html>
)rawliteral";

// Mengontrol servo dari slider web
void handleServo() {
  if (!server.hasArg("pos") ||
      !isNumeric(server.arg("pos"))) {
    server.send(
      400,
      "text/plain",
      "Input tidak valid (harus angka 0-180)!"
    );
    return;
  }

  int pos = server.arg("pos").toInt();

  if (pos < 0 || pos > 180) {
    server.send(
      400,
      "text/plain",
      "Sudut di luar batas (0-180)!"
    );
    return;
  }

  currentServoPos = pos;
  myServo.write(currentServoPos);

  Serial.println(
    "[WEB] Servo diubah manual ke: " +
    String(currentServoPos) + " derajat"
  );

  server.send(
    200,
    "text/plain",
    "Servo diatur ke " + String(pos) + " derajat"
  );
}

// Mengirim data sensor dalam format JSON
void handleData() {
  String json =
    "{\"sudut\":" + String(sudutTerakhir, 2) +
    ",\"accY\":" + String(accYTerakhir, 2) +
    ",\"status\":\"" + statusTerakhir + "\"" +
    ",\"buzzer\":\"" + buzzerTerakhir + "\"" +
    ",\"servo\":" + String(currentServoPos) + "}";

  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);

  // Inisialisasi buzzer
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  // Inisialisasi servo
  myServo.attach(servoPin);
  myServo.write(currentServoPos);

  // Inisialisasi sensor MPU6050
  Wire.begin(21, 22);

  if (!mpu.begin()) {
    Serial.println(
      "Gagal menemukan sensor MPU6050! "
      "Cek kabel SDA(21) dan SCL(22)."
    );

    while (1) {
      delay(10);
    }
  }

  Serial.println("MPU6050 Siap!");

  // Pengaturan sensor
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // Menghubungkan ESP32 ke Wi-Fi
  WiFi.begin(ssid, password);

  Serial.print("Menghubungkan ke WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println(
    "Terhubung! IP Address: " +
    WiFi.localIP().toString()
  );

  // Membaca kondisi awal sensor
  cekKondisiMPU();

  // Endpoint halaman dan data
  server.on("/", []() {
    server.send(200, "text/html", index_html);
  });

  server.on("/servo", handleServo);
  server.on("/data", handleData);

  server.begin();
  Serial.println("Web server berjalan!");
}

void loop() {
  server.handleClient();

  unsigned long waktuSekarang = millis();

  if (waktuSekarang - waktuSebelumnya >= interval) {
    waktuSebelumnya = waktuSekarang;

    cekKondisiMPU();
  }
=======
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>

const char* ssid = "tum";
const char* password = "nadinerev";

WebServer server(80);
Servo myServo;
Adafruit_MPU6050 mpu;

// Pin ESP32
const int servoPin = 18;
const int buzzerPin = 23;

// Variabel sistem
int currentServoPos = 90;
float accYTerakhir = 0.0;
float sudutTerakhir = 0.0;
String statusTerakhir = "";
String buzzerTerakhir = "OFF";

unsigned long waktuSebelumnya = 0;
const long interval = 2000;

// Memeriksa input angka
bool isNumeric(String str) {
  if (str.length() == 0) return false;

  for (unsigned int i = 0; i < str.length(); i++) {
    if (!isDigit(str.charAt(i))) return false;
  }

  return true;
}

// Membaca sudut kemiringan MPU6050
float bacaKemiringan(float &nilaiAccelY) {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  nilaiAccelY = a.acceleration.y;

  float roll = atan2(
    a.acceleration.y,
    a.acceleration.z
  ) * 180.0 / PI;

  return fabs(roll);
}

// Pemeriksaan kondisi sensor
void cekKondisiMPU() {
  sudutTerakhir = bacaKemiringan(accYTerakhir);

  String statusBaru = "";

  // Status BAHAYA
  if (sudutTerakhir > 35.0) {
    statusBaru = "BAHAYA";
    buzzerTerakhir = "ON (MENYALA)";

    digitalWrite(buzzerPin, HIGH);

    currentServoPos = 180;
    myServo.write(currentServoPos);
  }

  // Status SEDANG
  else if (sudutTerakhir >= 15.0) {
    statusBaru = "SEDANG";
    buzzerTerakhir = "OFF";

    digitalWrite(buzzerPin, LOW);

    // Servo bergerak hanya saat status berubah
    if (statusTerakhir != "SEDANG") {
      currentServoPos = 90;
      myServo.write(currentServoPos);
    }
  }

  // Status DATAR
  else {
    statusBaru = "DATAR";
    buzzerTerakhir = "OFF";

    digitalWrite(buzzerPin, LOW);

    // Servo bergerak hanya saat status berubah
    if (statusTerakhir != "DATAR") {
      currentServoPos = 0;
      myServo.write(currentServoPos);
    }
  }

  statusTerakhir = statusBaru;

  // Informasi di Serial Monitor
  Serial.print("Accel Y: ");
  Serial.print(accYTerakhir, 2);
  Serial.print(" m/s2 | Sudut: ");
  Serial.print(sudutTerakhir, 1);
  Serial.print(" derajat | Kondisi: ");
  Serial.print(statusTerakhir);
  Serial.print(" | Buzzer: ");
  Serial.print(buzzerTerakhir);
  Serial.print(" | Servo: ");
  Serial.print(currentServoPos);
  Serial.println(" derajat");
}

// Halaman web dashboard
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Tugas 3 - MPU6050 + Servo + Buzzer</title>

<style>
body {
  font-family: Arial, sans-serif;
  text-align: center;
  margin-top: 30px;
  padding: 0 10px;
}

.badge {
  padding: 12px 25px;
  color: white;
  font-weight: bold;
  border-radius: 8px;
  display: inline-block;
  font-size: 20px;
}

.DATAR {
  background-color: #6c757d;
}

.SEDANG {
  background-color: #28a745;
}

.BAHAYA {
  background-color: #dc3545;
}

.card {
  border: 1px solid #ccc;
  padding: 20px;
  margin: 15px auto;
  max-width: 340px;
  border-radius: 10px;
}

input[type=range] {
  width: 90%;
}
</style>
</head>

<body>
<h2>Sistem Otomatis Kemiringan
(MPU6050, Servo & Buzzer)</h2>

<div class="card">
  <h3>Status Sensor MPU6050</h3>

  <p>
    Kemiringan: <span id="sudut">--</span> &deg;
    (Accel Y: <span id="accY">--</span> m/s&sup2;)
  </p>

  <div id="statusBox" class="badge SEDANG">
    MEMUAT...
  </div>

  <p>Buzzer: <b id="buzzerStatus">OFF</b></p>
</div>

<div class="card">
  <h3>Kendali Servo</h3>

  <p>Sudut Servo: <span id="posVal">90</span>&deg;</p>

  <input
    type="range"
    min="0"
    max="180"
    value="90"
    id="servoSlider"
    oninput="updateServo(this.value)"
  >

  <p id="statusMsg"></p>
</div>

<script>
function updateServo(val) {
  document.getElementById("posVal").innerText = val;

  fetch("/servo?pos=" + val)
    .then(res =>
      res.text().then(text => ({
        status: res.status,
        body: text
      }))
    )
    .then(res => {
      document.getElementById("statusMsg").innerText =
        "Status " + res.status + ": " + res.body;
    })
    .catch(() => {
      document.getElementById("statusMsg").innerText =
        "Gagal menghubungi ESP32";
    });
}

function ambilData() {
  fetch("/data")
    .then(res => res.json())
    .then(data => {
      document.getElementById("sudut").innerText =
        data.sudut.toFixed(1);

      document.getElementById("accY").innerText =
        data.accY.toFixed(2);

      let box = document.getElementById("statusBox");
      box.innerText = data.status;
      box.className = "badge " + data.status;

      document.getElementById("buzzerStatus").innerText =
        data.buzzer;

      document.getElementById("posVal").innerText =
        data.servo;

      document.getElementById("servoSlider").value =
        data.servo;
    })
    .catch(err => console.log("Gagal mengambil data:", err));
}

setInterval(ambilData, 2000);
ambilData();
</script>

</body>
</html>
)rawliteral";

// Mengontrol servo dari slider web
void handleServo() {
  if (!server.hasArg("pos") ||
      !isNumeric(server.arg("pos"))) {
    server.send(
      400,
      "text/plain",
      "Input tidak valid (harus angka 0-180)!"
    );
    return;
  }

  int pos = server.arg("pos").toInt();

  if (pos < 0 || pos > 180) {
    server.send(
      400,
      "text/plain",
      "Sudut di luar batas (0-180)!"
    );
    return;
  }

  currentServoPos = pos;
  myServo.write(currentServoPos);

  Serial.println(
    "[WEB] Servo diubah manual ke: " +
    String(currentServoPos) + " derajat"
  );

  server.send(
    200,
    "text/plain",
    "Servo diatur ke " + String(pos) + " derajat"
  );
}

// Mengirim data sensor dalam format JSON
void handleData() {
  String json =
    "{\"sudut\":" + String(sudutTerakhir, 2) +
    ",\"accY\":" + String(accYTerakhir, 2) +
    ",\"status\":\"" + statusTerakhir + "\"" +
    ",\"buzzer\":\"" + buzzerTerakhir + "\"" +
    ",\"servo\":" + String(currentServoPos) + "}";

  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);

  // Inisialisasi buzzer
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  // Inisialisasi servo
  myServo.attach(servoPin);
  myServo.write(currentServoPos);

  // Inisialisasi sensor MPU6050
  Wire.begin(21, 22);

  if (!mpu.begin()) {
    Serial.println(
      "Gagal menemukan sensor MPU6050! "
      "Cek kabel SDA(21) dan SCL(22)."
    );

    while (1) {
      delay(10);
    }
  }

  Serial.println("MPU6050 Siap!");

  // Pengaturan sensor
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // Menghubungkan ESP32 ke Wi-Fi
  WiFi.begin(ssid, password);

  Serial.print("Menghubungkan ke WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println(
    "Terhubung! IP Address: " +
    WiFi.localIP().toString()
  );

  // Membaca kondisi awal sensor
  cekKondisiMPU();

  // Endpoint halaman dan data
  server.on("/", []() {
    server.send(200, "text/html", index_html);
  });

  server.on("/servo", handleServo);
  server.on("/data", handleData);

  server.begin();
  Serial.println("Web server berjalan!");
}

void loop() {
  server.handleClient();

  unsigned long waktuSekarang = millis();

  if (waktuSekarang - waktuSebelumnya >= interval) {
    waktuSebelumnya = waktuSekarang;

    cekKondisiMPU();
  }
>>>>>>> cac8d5a9671a968f025fd13ed44a9e9df74086a1
}