#include <WiFi.h>
#include <WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPIFFS.h>

const char* ssid = "Ram Raksha";
const char* password = "7383679291";

#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

WebServer server(80);
const char* filePath = "/temperature.csv";

unsigned long previousMillis = 0;
const long interval = 3000;

float currentTemperature = 0.0;

// ================= GENERATE TABLE FROM CSV =================
String generateTable() {
  File file = SPIFFS.open(filePath, FILE_READ);
  String table = "<table border='1' style='margin:auto; border-collapse:collapse;'>";
  table += "<tr><th>Time (ms)</th><th>Temperature (°C)</th></tr>";

  while (file.available()) {
    String line = file.readStringUntil('\n');
    int commaIndex = line.indexOf(',');
    if (commaIndex > 0) {
      String timeVal = line.substring(0, commaIndex);
      String tempVal = line.substring(commaIndex + 1);
      table += "<tr><td>" + timeVal + "</td><td>" + tempVal + "</td></tr>";
    }
  }

  table += "</table>";
  file.close();
  return table;
}

void setup() {
  Serial.begin(115200);
  sensors.begin();

  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Failed");
    return;
  }

  // Clear file every new session
  File file = SPIFFS.open(filePath, FILE_WRITE);
  file.println("Time(ms),Temperature(C)");
  file.close();

  WiFi.begin(ssid, password);
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("Open in browser: http://");
  Serial.println(WiFi.localIP());

  server.on("/", []() {
    String page = "<!DOCTYPE html><html><head>";
    page += "<meta http-equiv='refresh' content='3'>";
    page += "<style>body{text-align:center;font-family:Arial;} ";
    page += ".temp{font-size:50px;color:red;} ";
    page += "table{margin-top:20px;} th,td{padding:8px;}</style>";
    page += "</head><body>";
    page += "<h2>Table Fan Predictive Maintenance</h2>";
    page += "<div class='temp'>" + String(currentTemperature) + " °C</div><br>";
    page += "<a href='/download'><button>Download CSV</button></a>";
    page += "<h3>Session Readings</h3>";
    page += generateTable();
    page += "</body></html>";

    server.send(200, "text/html", page);
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
    currentTemperature = sensors.getTempCByIndex(0);

    Serial.print("Temperature: ");
    Serial.println(currentTemperature);

    File file = SPIFFS.open(filePath, FILE_APPEND);
    if (file) {
      file.print(currentMillis);
      file.print(",");
      file.println(currentTemperature);
      file.close();
    }
  }
}