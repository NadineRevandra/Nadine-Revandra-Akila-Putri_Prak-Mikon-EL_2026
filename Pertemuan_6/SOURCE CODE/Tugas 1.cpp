#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

const char* ssid = "IphoNe";
const char* password = "Neysahana123";

WebServer server(80);
Servo myServo;

const int servoPin = 18;

// Fungsi untuk memastikan string hanya berisi digit angka
bool isNumeric(String str) {
  if (str.length() == 0) return false;
  for (unsigned int i = 0; i < str.length(); i++) {
    if (!isDigit(str.charAt(i))) return false;
  }
  return true;
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Tugas 1 - Kontrol Servo</title>
<style>
body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }
input[type=range] { width: 300px; }
</style>
</head>
<body>
<h2>Kendali Servo ESP32</h2>
<p>Sudut: <span id="posVal">90</span>&deg;</p>
<input type="range" min="0" max="180" value="90" id="servoSlider" oninput="updateServo(this.value)">
<p id="statusMsg"></p>
<script>
function updateServo(val) {
  document.getElementById("posVal").innerText = val;
  fetch("/servo?pos=" + val) // (1) isi dengan path + nama parameter, diakhiri tanda '='
  .then(response => {
    return response.text().then(text => ({ status: response.status, body: text }));
  })
  .then(res => {
    document.getElementById("statusMsg").innerText = `Status ${res.status}: ${res.body}`;
  });
}
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleServo() {
  if (!server.hasArg("pos")) {
    server.send(400, "text/plain", "Parameter 'pos' tidak ditemukan");
    return;
  }
  
  String posStr = server.arg("pos"); // (2) nama parameter yang dikirim dari halaman web
  
  if (!isNumeric(posStr)) {
    server.send(400, "text/plain", "Input tidak valid (harus angka 0-180)");
    return;
  }
  
  int pos = posStr.toInt();
  
  if (pos < 0 || pos > 180) { // (3) kondisi ketika sudut di luar rentang 0-180
    server.send(400, "text/plain", "Sudut di luar batas (harus 0-180)");
    return;
  }
  
  myServo.write(pos); // (4) gerakkan myServo ke sudut pos
  
  server.send(200, "text/plain", "Servo bergerak ke " + String(pos) + " derajat"); // (5) status code untuk permintaan yang berhasil
}

void setup() {
  Serial.begin(115200);
  
  myServo.attach(servoPin); // (6) hubungkan myServo ke servoPin
  myServo.write(90);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nTerhubung! IP Address: " + WiFi.localIP().toString());

  server.on("/", handleRoot);
  server.on("/servo", handleServo); // (7) path yang dipanggil fetch() pada isian (1)
  server.begin();
}

void loop() {
  server.handleClient();
}