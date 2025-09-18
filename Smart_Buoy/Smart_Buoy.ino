#include <Wire.h>
#include <MPU6050_light.h>
#include <Adafruit_BMP280.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

// ---------- CONFIG ----------
const float VOLTAGE_DIVIDER_RATIO = 0.203; // Adjust for your resistor divider

// Pin mapping (NodeMCU / ESP-12E)
const int PIN_ULTRA_TRIG = D6; // GPIO12
const int PIN_ULTRA_ECHO = D7; // GPIO13
const int PIN_BAT_ADC   = A0;  // ADC pin

// ---------- SENSORS ----------
MPU6050 mpu(Wire);
Adafruit_BMP280 bmp; // I2C
bool bmp_ok = false; // flag for BMP init

// ---------- Helpers ----------
float readUltrasonicDistance() {
  digitalWrite(PIN_ULTRA_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_ULTRA_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_ULTRA_TRIG, LOW);

  unsigned long duration = pulseIn(PIN_ULTRA_ECHO, HIGH, 30000UL); // 30 ms timeout
  if (duration == 0) return -1.0;
  float soundSpeed = 343.0; // m/s at ~20 °C (adjust with temp if needed)
  return (duration / 1e6 * soundSpeed) / 2.0;
}

float readBatteryVoltage() {
  int raw = analogRead(PIN_BAT_ADC);  // 0..1023
  float v_adc = (raw / 1023.0) * 3.3; // NodeMCU maps to 0–3.3V
  return v_adc / VOLTAGE_DIVIDER_RATIO;
}

// ---------- LittleFS helpers ----------
bool initLittleFS() {
  if (!LittleFS.begin()) {
    Serial.println("LittleFS.begin() failed");
    return false;
  }
  return true;
}

bool saveJsonToFile(const String &jsonStr, const char *path) {
  File f = LittleFS.open(path, "w");
  if (!f) {
    Serial.printf("Failed to open %s for writing\n", path);
    return false;
  }
  f.print(jsonStr);
  f.close();
  return true;
}

String readFileFromLittleFS(const char *path) {
  if (!LittleFS.exists(path)) return String();
  File f = LittleFS.open(path, "r");
  if (!f) return String();
  String s;
  while (f.available()) s += (char)f.read();
  f.close();
  return s;
}

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);
  delay(200);

  // I2C pins
  Wire.begin(D2, D1); // SDA=D2(GPIO4), SCL=D1(GPIO5)

  // MPU6050
  byte status = mpu.begin();
  if (status != 0) {
    Serial.printf("MPU6050 init failed (code %d)\n", status);
  } else {
    Serial.println("MPU6050 OK");
  }

  // BMP280
  if (!bmp.begin(0x76)) {
    Serial.println("BMP280 init failed (try 0x77)");
    bmp_ok = false;
  } else {
    Serial.println("BMP280 OK");
    bmp_ok = true;
  }

  pinMode(PIN_ULTRA_TRIG, OUTPUT);
  pinMode(PIN_ULTRA_ECHO, INPUT);

  // Init LittleFS
  if (initLittleFS()) {
    Serial.println("LittleFS mounted");
  } else {
    Serial.println("LittleFS mount failed");
  }
}

// ---------- Loop ----------
void loop() {
  mpu.update();

  // MPU6050 values
  float accX = mpu.getAccX();
  float accY = mpu.getAccY();
  float accZ = mpu.getAccZ();
  float gyroX = mpu.getGyroX();
  float gyroY = mpu.getGyroY();
  float gyroZ = mpu.getGyroZ();

  // Magnitude of acceleration (proxy for motion/waves)
  float acc_mag = sqrt(accX * accX + accY * accY + accZ * accZ);

  // BMP280 values
  float tempC = NAN, pres_hPa = NAN;
  if (bmp_ok) {
    tempC = bmp.readTemperature();
    pres_hPa = bmp.readPressure() / 100.0;
  }

  // Ultrasonic distance
  float level_m = readUltrasonicDistance();

  // Battery voltage
  float vbat = readBatteryVoltage();

  // ---------- Build main JSON (everything except gyro) ----------
  StaticJsonDocument<512> mainDoc;
  mainDoc["device_id"] = "buoy001";
  mainDoc["ts_ms"] = millis();

  JsonObject motion = mainDoc.createNestedObject("motion");
  motion["acc_x"] = accX;
  motion["acc_y"] = accY;
  motion["acc_z"] = accZ;
  motion["acc_mag"] = acc_mag;
  // NOTE: gyro removed from mainDoc

  if (level_m >= 0) mainDoc["lvl_m"] = level_m;
  else mainDoc["lvl_m"] = nullptr;

  if (!isnan(tempC)) {
    JsonObject weather = mainDoc.createNestedObject("weather");
    weather["t_c"] = tempC;
    weather["p_hPa"] = pres_hPa;
  } else {
    mainDoc["weather"] = nullptr;
  }

  mainDoc["vbat"] = vbat;

  String mainOutput;
  serializeJson(mainDoc, mainOutput);

  // ---------- Build gyro-only JSON ----------
  StaticJsonDocument<256> gyroDoc;
  gyroDoc["device_id"] = "buoy001";
  gyroDoc["ts_ms"] = millis();
  JsonObject gyro = gyroDoc.createNestedObject("gyro");
  gyro["gx"] = gyroX;
  gyro["gy"] = gyroY;
  gyro["gz"] = gyroZ;

  String gyroOutput;
  serializeJson(gyroDoc, gyroOutput);

  // ---------- Print to Serial ----------
  Serial.println("----- Main JSON (/data.json) -----");
  Serial.println(mainOutput);
  Serial.println("----------------------------------");
  Serial.println("----- Gyro JSON (/gyro.json) -----");
  Serial.println(gyroOutput);
  Serial.println("----------------------------------");

  // ---------- Save both files ----------
  bool okMain = saveJsonToFile(mainOutput, "/data.json");
  bool okGyro = saveJsonToFile(gyroOutput, "/gyro.json");

  Serial.printf("Saved /data.json: %s, /gyro.json: %s\n", okMain ? "OK" : "FAIL", okGyro ? "OK" : "FAIL");
  
  // delay 20s
  delay(20000);
}
