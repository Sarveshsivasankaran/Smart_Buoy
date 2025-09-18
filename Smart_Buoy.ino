#include <Wire.h>
#include <MPU6050_light.h>
#include <Adafruit_BMP280.h>

// ---------- CONFIG ----------
const float VOLTAGE_DIVIDER_RATIO = 0.203; // Adjust for your resistor divider

// Pin mapping (NodeMCU / ESP-12E)
const int PIN_ULTRA_TRIG = D6; // GPIO12
const int PIN_ULTRA_ECHO = D7; // GPIO13
const int PIN_BAT_ADC   = A0;  // ADC pin

// ---------- SENSORS ----------
MPU6050 mpu(Wire);
Adafruit_BMP280 bmp; // I2C

// ---------- Helpers ----------
float readUltrasonicDistance() {
  digitalWrite(PIN_ULTRA_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_ULTRA_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_ULTRA_TRIG, LOW);

  unsigned long duration = pulseIn(PIN_ULTRA_ECHO, HIGH, 30000UL); // 30 ms timeout
  if (duration == 0) return -1.0;
  float soundSpeed = 343.0; // m/s at ~20 °C
  return (duration / 1e6 * soundSpeed) / 2.0;
}

float readBatteryVoltage() {
  int raw = analogRead(PIN_BAT_ADC);  // 0..1023
  float v_adc = (raw / 1023.0) * 3.3; // NodeMCU maps to 0–3.3V
  return v_adc / VOLTAGE_DIVIDER_RATIO;
}

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);
  delay(200);

  Wire.begin(D2, D1); // SDA=D2(GPIO4), SCL=D1(GPIO5)

  // MPU6050
  byte status = mpu.begin();
  if (status != 0) {
    Serial.printf("MPU6050 init failed (code %d)\n", status);
  } else {
    Serial.println("MPU6050 OK");
  }
  bool bmp_ok=false;
  // BMP280
  if (!bmp.begin(0x76)) {
    Serial.println("BMP280 init failed (try 0x77)");
  } else {
    Serial.println("BMP280 OK");
    bmp_ok=true;
  }

  pinMode(PIN_ULTRA_TRIG, OUTPUT);
  pinMode(PIN_ULTRA_ECHO, INPUT);
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
  if (bmp.getStatus()) {
    tempC = bmp.readTemperature();
    pres_hPa = bmp.readPressure() / 100.0;
  }

  // Ultrasonic distance
  float level_m = readUltrasonicDistance();

  // Battery voltage
  float vbat = readBatteryVoltage();

  // ---- Print all values ----
  Serial.println("---- Sensor Data ----");
  Serial.printf("Accel (X,Y,Z): %.3f, %.3f, %.3f g\n", accX, accY, accZ);
  Serial.printf("Gyro  (X,Y,Z): %.3f, %.3f, %.3f deg/s\n", gyroX, gyroY, gyroZ);
  Serial.printf("Accel magnitude: %.3f g\n", acc_mag);

  if (!isnan(tempC)) {
    Serial.printf("Temperature: %.2f C\n", tempC);
    Serial.printf("Pressure: %.2f hPa\n", pres_hPa);
  } else {
    Serial.println("BMP280 not found");
  }

  if (level_m >= 0) {
    Serial.printf("Ultrasonic water level: %.3f m\n", level_m);
  } else {
    Serial.println("Ultrasonic: no reading");
  }

  Serial.printf("Battery voltage: %.2f V\n", vbat);
  Serial.println("---------------------\n");

  delay(20000); // 2s refresh
}
