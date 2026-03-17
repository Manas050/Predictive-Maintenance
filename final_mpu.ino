#include <WiFi.h>
#include <WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPIFFS.h>

// ================= WIFI =================
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// ================= DS18B20 =================
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// ================= WEB SERVER =================
WebServer server(80);
const char* filePath = "/temperature.csv";

// ================= TIMER =================
unsigned long previousMillis = 0;
const long interval = 3000;

// ================= HTML PAGE =================
String webpage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<title>Fan Temperature Monitor</title>
<meta http-equiv="refresh" content="5">
</head>
<body>
<h2>Table Fan Predictive Maintenance</h2>
<p>Temperature logging every 3 seconds.</p>
<a href="/download"><button>Download CSV</button></a>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);

  sensors.begin();

  // SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Failed");
    return;
  }

  // Create file if not exists
  if (!SPIFFS.exists(filePath)) {
    File file = SPIFFS.open(filePath, FILE_WRITE);
    file.println("Time(ms),Temperature(C)");
    file.close();
  }

  // WiFi connect
  WiFi.begin(ssid, password);
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Routes
  server.on("/", []() {
    server.send(200, "text/html", webpage);
  });

  server.on("/download", []() {
    File file = SPIFFS.open(filePath, FILE_READ);
    server.streamFile(file, "text/csv");
    file.close();
  });

  server.begin();
}

void loop() {
  server.handleClient();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    sensors.requestTemperatures();
    float temperature = sensors.getTempCByIndex(0);

    Serial.print("Temperature: ");
    Serial.println(temperature);

    File file = SPIFFS.open(filePath, FILE_APPEND);
    if (file) {
      file.print(currentMillis);
      file.print(",");
      file.println(temperature);
      file.close();
    }
  }
}