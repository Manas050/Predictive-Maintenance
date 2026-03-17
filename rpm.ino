#include <WiFi.h>
#include <WebServer.h>

#define HALL_SENSOR_PIN 4

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

WebServer server(80);

volatile unsigned long pulseCount = 0;
unsigned long lastTime = 0;
unsigned long lastCSVUpdate = 0;

float rpm = 0;

String csvData = "Time(ms),RPM\n";

void IRAM_ATTR countPulse() {
  pulseCount++;
}

void calculateRPM() {
  unsigned long currentTime = millis();
  unsigned long timeDiff = currentTime - lastTime;

  if (timeDiff >= 3000) {  // Every 3 seconds
    noInterrupts();
    unsigned long count = pulseCount;
    pulseCount = 0;
    interrupts();

    // 1 pulse per revolution
    rpm = (count * 60000.0) / timeDiff;

    lastTime = currentTime;

    // Append to CSV
    csvData += String(currentTime) + "," + String(rpm) + "\n";

    Serial.print("RPM: ");
    Serial.println(rpm);
  }
}

void handleRoot() {
  String page = "<html><head>";
  page += "<meta http-equiv='refresh' content='3'>";
  page += "<title>Fan Predictive Maintenance</title></head><body>";
  page += "<h2>Table Fan RPM Monitoring</h2>";
  page += "<h3>Current RPM: " + String(rpm) + "</h3>";
  page += "<br><a href='/download'>Download CSV</a>";
  page += "</body></html>";

  server.send(200, "text/html", page);
}

void handleDownload() {
  server.sendHeader("Content-Disposition", "attachment; filename=fan_data.csv");
  server.send(200, "text/csv", csvData);
}

void setup() {
  Serial.begin(115200);

  pinMode(HALL_SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(HALL_SENSOR_PIN), countPulse, FALLING);

  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("Open this IP in browser: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/download", handleDownload);

  server.begin();

  lastTime = millis();
}

void loop() {
  server.handleClient();
  calculateRPM();
}