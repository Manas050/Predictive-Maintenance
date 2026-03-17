#include <WiFi.h>
#include <WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <MPU6050.h>

// ================= WIFI =================
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// ================= DS18B20 =================
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// ================= HALL SENSOR =================
#define HALL_PIN 5
volatile unsigned long pulseCount = 0;
unsigned long lastRPMTime = 0;
float rpm = 0;

// ================= MPU6050 =================
MPU6050 mpu;
float vibration = 0;

// ================= WEB SERVER =================
WebServer server(80);
const char* filePath = "/data.csv";

// ================= TIMER =================
unsigned long previousMillis = 0;
const long interval = 3000;

// ================= GLOBAL DATA =================
float temperature = 0;

// ================= INTERRUPT =================
void IRAM_ATTR countPulse() {
  pulseCount++;
}

// ================= HTML PAGE =================
String getPage() {
  String page = "<html><head>";
  page += "<meta http-equiv='refresh' content='3'>";
  page += "<style>body{text-align:center;font-family:Arial;} ";
  page += ".big{font-size:30px;color:blue;}</style>";
  page += "</head><body>";

  page += "<h2>Fan Predictive Maintenance</h2>";
  page += "<div class='big'>Temp: " + String(temperature) + " °C</div>";
  page += "<div class='big'>RPM: " + String(rpm) + "</div>";
  page += "<div class='big'>Vibration: " + String(vibration) + "</div>";

  page += "<br><a href='/download'><button>Download CSV</button></a>";
  page += "</body></html>";

  return page;
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  // Sensors init
  sensors.begin();

  Wire.begin(21, 22);
  mpu.initialize();

  pinMode(HALL_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(HALL_PIN), countPulse, FALLING);

  // SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Failed");
    return;
  }

  // Create CSV
  File file = SPIFFS.open(filePath, FILE_WRITE);
  file.println("Time(ms),Temp(C),RPM,Vibration");
  file.close();

  // WiFi connect
  WiFi.begin(ssid, password);
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("OPEN THIS IN BROWSER: http://");
  Serial.println(WiFi.localIP());

  // Routes
  server.on("/", []() {
    server.send(200, "text/html", getPage());
  });

  server.on("/download", []() {
    File file = SPIFFS.open(filePath, FILE_READ);
    server.streamFile(file, "text/csv");
    file.close();
  });

  server.begin();
}

// ================= LOOP =================
void loop() {
  server.handleClient();

  unsigned long currentMillis = millis();

  // ===== RPM CALCULATION =====
  if (currentMillis - lastRPMTime >= 3000) {
    noInterrupts();
    unsigned long count = pulseCount;
    pulseCount = 0;
    interrupts();

    rpm = (count * 60000.0) / (currentMillis - lastRPMTime);
    lastRPMTime = currentMillis;
  }

  // ===== SENSOR LOGGING =====
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Temperature
    sensors.requestTemperatures();
    temperature = sensors.getTempCByIndex(0);

    // MPU6050 vibration (magnitude)
    int16_t ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);
    vibration = sqrt(ax * ax + ay * ay + az * az) / 16384.0;

    // Serial output
    Serial.print("Temp: "); Serial.print(temperature);
    Serial.print(" | RPM: "); Serial.print(rpm);
    Serial.print(" | Vib: "); Serial.println(vibration);

    // Save CSV
    File file = SPIFFS.open(filePath, FILE_APPEND);
    if (file) {
      file.print(currentMillis);
      file.print(",");
      file.print(temperature);
      file.print(",");
      file.print(rpm);
      file.print(",");
      file.println(vibration);
      file.close();
    }
  }
}