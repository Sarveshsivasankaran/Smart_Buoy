// Smart Buoy -> Supabase inserts (schema-matching, static buoy_name BOUY12)
// Uses: MPU6050_light, Adafruit_BMP280, ArduinoJson, LittleFS, ESP8266WiFi, ESP8266HTTPClient

#include <Wire.h>
#include <MPU6050_light.h>
#include <Adafruit_BMP280.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

// ---------------- CONFIG ----------------
const char* WIFI_SSID = "Xx";
const char* WIFI_PASS = "sarvesh@2643";

const char* SUPABASE_URL = "https://emsnpqgknwitsokfkyhp.supabase.co";
const char* SUPABASE_KEY = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImVtc25wcWdrbndpdHNva2ZreWhwIiwicm9sZSI6InNlcnZpY2Vfcm9sZSIsImlhdCI6MTc1NzQyMTc2NywiZXhwIjoyMDcyOTk3NzY3fQ.xJuE3aiTSnIdqSmmxVANLLWqovvMCpZUSFTw7x_3_FA";

const char* ENDPOINT_MINUTE = "/rest/v1/buoy_minute_data";
const char* ENDPOINT_SECOND = "/rest/v1/buoy_second_data";

const char* STATIC_BUOY_NAME = "BOUY12"; // requested static name

// hardware
const float VOLTAGE_DIVIDER_RATIO = 0.203;
const int PIN_ULTRA_TRIG = D6; // GPIO12
const int PIN_ULTRA_ECHO = D7; // GPIO13
const int PIN_BAT_ADC   = A0;  // ADC pin

// sensors
MPU6050 mpu(Wire);
Adafruit_BMP280 bmp;
bool bmp_ok = false;

// timings
const unsigned long TABLE2_INTERVAL_MS = 1000UL;   // 1s
const unsigned long TABLE1_INTERVAL_MS = 60000UL;  // 60s
unsigned long lastTable2Millis = 0;
unsigned long lastTable1Millis = 0;

// persisted uuid for buoy (from DB)
String buoy_id = "";

// ---------------- LittleFS helpers ----------------
bool initLittleFS() {
  if (!LittleFS.begin()) { Serial.println("LittleFS.begin failed"); return false; }
  return true;
}
bool saveStringToFS(const String &s, const char *path) {
  File f = LittleFS.open(path, "w"); if (!f) return false; f.print(s); f.close(); return true;
}
String readStringFromFS(const char *path) {
  if (!LittleFS.exists(path)) return String();
  File f = LittleFS.open(path, "r"); if (!f) return String();
  String s; while (f.available()) s += (char)f.read();
  f.close(); return s;
}

// ---------------- WiFi helpers ----------------
bool ensureWifiConnected(unsigned long timeoutMs = 15000) {
  if (WiFi.status() == WL_CONNECTED) return true;
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  unsigned long start = millis();
  while (millis() - start < timeoutMs) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("WiFi IP: "); Serial.println(WiFi.localIP());
      return true;
    }
    delay(200);
  }
  Serial.println("WiFi connect timeout");
  return false;
}

// ---------------- sensors ----------------
float readUltrasonicDistance() {
  digitalWrite(PIN_ULTRA_TRIG, LOW); delayMicroseconds(2);
  digitalWrite(PIN_ULTRA_TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(PIN_ULTRA_TRIG, LOW);
  unsigned long duration = pulseIn(PIN_ULTRA_ECHO, HIGH, 30000UL);
  if (duration == 0) return -1.0;
  float soundSpeed = 343.0;
  return (duration / 1e6 * soundSpeed) / 2.0;
}
float readBatteryVoltage() {
  int raw = analogRead(PIN_BAT_ADC);
  float v_adc = (raw / 1023.0) * 3.3;
  return v_adc / VOLTAGE_DIVIDER_RATIO;
}

// ---------------- HTTP / Supabase helpers ----------------
String buildUrl(const char* endpoint) {
  return String(SUPABASE_URL) + String(endpoint);
}

// POST JSON to given REST path; returns HTTP code and populates responseStr
int supabasePost(const String &jsonStr, const char *restPath, String &responseStr) {
  if (!ensureWifiConnected()) return -1;
  BearSSL::WiFiClientSecure client; client.setInsecure(); // prototype only
  HTTPClient http;
  String url = buildUrl(restPath);
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("apikey", SUPABASE_KEY);
  http.addHeader("Authorization", String("Bearer ") + SUPABASE_KEY);
  http.addHeader("Prefer", "return=representation");
  int code = http.POST((uint8_t*)jsonStr.c_str(), jsonStr.length());
  if (code > 0) responseStr = http.getString();
  else responseStr = String();
  http.end();
  return code;
}

// GET (simple) - returns HTTP code and resp string
int supabaseGet(const char* restQuery, String &responseStr) {
  if (!ensureWifiConnected()) return -1;
  BearSSL::WiFiClientSecure client; client.setInsecure();
  HTTPClient http;
  String url = String(SUPABASE_URL) + String(restQuery);
  http.begin(client, url);
  http.addHeader("apikey", SUPABASE_KEY);
  http.addHeader("Authorization", String("Bearer ") + SUPABASE_KEY);
  int code = http.GET();
  if (code > 0) responseStr = http.getString();
  else responseStr = String();
  http.end();
  return code;
}

// ---------------- DB logic ----------------

// Try to find buoy_id by buoy_name via GET: /rest/v1/buoy_minute_data?select=buoy_id&buoy_name=eq.<name>
bool getBuoyIdByName(const char* name, String &outBuoyId) {
  String q = String("/rest/v1/buoy_minute_data?select=buoy_id&buoy_name=eq.") + String(name);
  String resp;
  int code = supabaseGet(q.c_str(), resp);
  Serial.printf("GET buoy by name HTTP %d\n", code);
  if (code >= 200 && code < 300 && resp.length()) {
    // Expecting JSON array like [ {"buoy_id":"uuid"} ] or []
    StaticJsonDocument<512> doc;
    DeserializationError err = deserializeJson(doc, resp);
    if (err) {
      Serial.print("deserializeJson GET err: "); Serial.println(err.c_str());
      return false;
    }
    if (doc.is<JsonArray>() && doc.size() > 0) {
      JsonObject row = doc[0].as<JsonObject>();
      if (row.containsKey("buoy_id")) {
        outBuoyId = String(row["buoy_id"].as<const char*>());
        return true;
      }
    }
  }
  return false;
}

// Insert minute row (with buoy_name + lat/long). If successful returns true and stores returned buoy_id if present.
bool insertMinuteRow(double gyro_x, double gyro_y, double gyro_z,
                     double temperature, double pressure, double battery_voltage,
                     double latitude, double longitude, const char* buoyName) {

  StaticJsonDocument<512> doc;
  doc["gyro_x"] = gyro_x;
  doc["gyro_y"] = gyro_y;
  doc["gyro_z"] = gyro_z;
  doc["temperature"] = temperature;
  doc["pressure"] = pressure;
  doc["battery_voltage"] = battery_voltage;
  doc["latitude"] = latitude;
  doc["longitude"] = longitude;
  doc["buoy_name"] = buoyName;

  String payload;
  serializeJson(doc, payload);
  Serial.println("Minute payload:"); Serial.println(payload);

  String resp;
  int code = supabasePost(payload, ENDPOINT_MINUTE, resp);
  Serial.printf("Minute POST code: %d\n", code);
  if (code >= 200 && code < 300 && resp.length()) {
    // Supabase returns representation array; parse for buoy_id
    StaticJsonDocument<768> j;
    DeserializationError err = deserializeJson(j, resp);
    if (!err && j.is<JsonArray>() && j.size() > 0) {
      JsonObject row = j[0].as<JsonObject>();
      if (row.containsKey("buoy_id")) {
        buoy_id = String(row["buoy_id"].as<const char*>());
        Serial.print("Saved buoy_id: "); Serial.println(buoy_id);
        saveStringToFS(buoy_id, "/buoy_id.txt");
      }
    }
    return true;
  } else {
    Serial.print("Minute insert failed resp: "); Serial.println(resp);
    return false;
  }
}

// Insert second row; must provide buoy_id and buoy_name (both FKs)
bool insertSecondRow(const String &buoyId, const char* buoyName,
                     double ax, double ay, double az, double accel_mag, double ultrasonic_level) {

  if (buoyId.length() == 0) { Serial.println("Missing buoy_id for second row"); return false; }

  StaticJsonDocument<384> doc;
  doc["buoy_id"] = buoyId;
  doc["buoy_name"] = buoyName;
  doc["accel_x"] = ax;
  doc["accel_y"] = ay;
  doc["accel_z"] = az;
  doc["accel_magnitude"] = accel_mag;
  doc["ultrasonic_water_level"] = ultrasonic_level;

  String payload; serializeJson(doc, payload);
  Serial.println("Second payload:"); Serial.println(payload);

  String resp; int code = supabasePost(payload, ENDPOINT_SECOND, resp);
  Serial.printf("Second POST code: %d\n", code);
  if (code >= 200 && code < 300) return true;
  Serial.print("Second insert failed resp: "); Serial.println(resp);
  // save failed payload for retry
  saveStringToFS(payload, "/failed_second.json");
  return false;
}

// ---------------- Setup & Loop ----------------
void setup() {
  Serial.begin(115200);
  delay(200);

  Wire.begin(D2, D1);
  byte st = mpu.begin();
  if (st != 0) Serial.printf("MPU6050 init failed (%d)\n", st); else Serial.println("MPU OK");

  if (!bmp.begin(0x76)) { Serial.println("BMP init failed"); bmp_ok = false; } else { bmp_ok = true; Serial.println("BMP OK"); }

  pinMode(PIN_ULTRA_TRIG, OUTPUT);
  pinMode(PIN_ULTRA_ECHO, INPUT);

  if (!initLittleFS()) Serial.println("LittleFS mount failed"); else Serial.println("LittleFS OK");

  // try load buoy_id
  String stored = readStringFromFS("/buoy_id.txt");
  if (stored.length()) {
    buoy_id = stored;
    Serial.print("Loaded buoy_id from FS: "); Serial.println(buoy_id);
  } else {
    Serial.println("No buoy_id in FS; will try GET by buoy_name");
  }

  // connect WiFi
  if (ensureWifiConnected()) Serial.println("WiFi ready");
  else Serial.println("WiFi not ready at setup");

  // if no buoy_id, try GET by name
  if (buoy_id.length() == 0) {
    String found;
    if (getBuoyIdByName(STATIC_BUOY_NAME, found)) {
      buoy_id = found;
      saveStringToFS(buoy_id, "/buoy_id.txt");
      Serial.print("Found buoy_id by name: "); Serial.println(buoy_id);
    } else {
      Serial.println("No existing buoy row found with buoy_name; will create when needed.");
    }
  }

  lastTable1Millis = millis();
  lastTable2Millis = millis();
}

void loop() {
  mpu.update();
  unsigned long now = millis();

  // constant coordinates for now (replace with GPS later)
  const double lat = 13.009742970148377;
  const double longg = 80.00504769188407;

  // Table2 every 1s: accel, acc_mag, ultrasonic
  if (now - lastTable2Millis >= TABLE2_INTERVAL_MS) {
    lastTable2Millis = now;

    float accX = mpu.getAccX();
    float accY = mpu.getAccY();
    float accZ = mpu.getAccZ();
    float acc_mag = sqrt(accX*accX + accY*accY + accZ*accZ);
    float level_m = readUltrasonicDistance();

    // backup locally (optional)
    StaticJsonDocument<256> t2;
    t2["device_id"] = STATIC_BUOY_NAME;
    t2["ts_ms"] = now;
    JsonObject a = t2.createNestedObject("accel");
    a["ax"]=accX; a["ay"]=accY; a["az"]=accZ;
    t2["accel_mag"] = acc_mag;
    if (level_m>=0) t2["lvl_m"]=level_m; else t2["lvl_m"]=nullptr;
    t2["latitude"]=lat; t2["longitude"]=longg;
    String t2s; serializeJson(t2, t2s); saveStringToFS(t2s, "/table2.json");

    // ensure buoy_id exists: if not, create minute row to get it
    if (buoy_id.length() == 0) {
      Serial.println("No buoy_id => creating minute placeholder to obtain buoy_id.");
      float vbat = readBatteryVoltage();
      float tmp = NAN, pres = NAN;
      if (bmp_ok) { tmp = bmp.readTemperature(); pres = bmp.readPressure()/100.0; }
      insertMinuteRow(0.0,0.0,0.0, isfinite(tmp)?tmp:0.0, isfinite(pres)?pres:0.0, vbat, lat, longg, STATIC_BUOY_NAME);
      // after this, buoy_id may be set (persisted by insertMinuteRow)
    }

    // if buoy_id now known, insert second row
    if (buoy_id.length()) {
      bool ok = insertSecondRow(buoy_id, STATIC_BUOY_NAME, accX, accY, accZ, acc_mag, (level_m>=0)?level_m:0.0);
      Serial.printf("Insert second row: %s\n", ok ? "OK":"FAIL");
    } else {
      Serial.println("Still missing buoy_id - skipping second insert");
    }
  }

  // Table1 every 1 minute: gyro + weather + battery + lat/long + buoy_name
  if (now - lastTable1Millis >= TABLE1_INTERVAL_MS) {
    lastTable1Millis = now;

    float gx = mpu.getGyroX();
    float gy = mpu.getGyroY();
    float gz = mpu.getGyroZ();
    float tmp = NAN, pres = NAN;
    if (bmp_ok) { tmp = bmp.readTemperature(); pres = bmp.readPressure()/100.0; }
    float vbat = readBatteryVoltage();

    // local backup
    StaticJsonDocument<384> t1;
    t1["device_id"] = STATIC_BUOY_NAME;
    t1["ts_ms"] = now;
    JsonObject g = t1.createNestedObject("gyro");
    g["gx"]=gx; g["gy"]=gy; g["gz"]=gz;
    JsonObject w = t1.createNestedObject("weather");
    if (isfinite(tmp)) w["t_c"]=tmp; else w["t_c"]=nullptr;
    if (isfinite(pres)) w["p_hPa"]=pres; else w["p_hPa"]=nullptr;
    t1["vbat"]=vbat; t1["latitude"]=lat; t1["longitude"]=longg; t1["buoy_name"]=STATIC_BUOY_NAME;
    String t1s; serializeJson(t1, t1s); saveStringToFS(t1s, "/table1.json");

    // Insert minute row (this will return representation and set buoy_id if new)
    bool ok = insertMinuteRow(gx, gy, gz, isfinite(tmp)?tmp:0.0, isfinite(pres)?pres:0.0, vbat, lat, longg, STATIC_BUOY_NAME);
    Serial.printf("Insert minute row: %s\n", ok ? "OK":"FAIL");
  }

  delay(10);
}
