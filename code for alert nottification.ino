#define BLYNK_TEMPLATE_ID "TMPL3sgFUX4Hd"
#define BLYNK_TEMPLATE_NAME "GYROSCOPE"
#define BLYNK_AUTH_TOKEN "2wu8TXKmitcEfn9mw7gmUyTI8y0fUBxv"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <MPU6050.h>

// ===== WiFi + Telegram =====
const char* ssid = "anwesha";
const char* password = "1234567890";

String botToken = "8262707313:AAEePSpe5ERsEGo0u4BdjoB43L7B_pY-Sec";
String chatID   = "7362669160";

// ===== MPU6050 =====
MPU6050 mpu;

// ===== Vibration Threshold =====
float threshold = 0.2;
unsigned long lastAlert = 0;
const unsigned long alertCooldown = 1000;

// ==================================================================
void sendTelegram(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.telegram.org/bot" + botToken +
                 "/sendMessage?chat_id=" + chatID +
                 "&text=" + message;
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) Serial.println("✅ Telegram Message Sent: " + message);
    else Serial.println("❌ Error sending Telegram message");
    http.end();
  }
}

// ==================================================================
void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  // WiFi + Blynk
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi connected ✅");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  // MPU6050
  Serial.println("Initializing MPU6050...");
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("❌ MPU6050 connection failed!");
    while (1);
  }
  Serial.println("✅ MPU6050 connected!");

  sendTelegram("ESP32 Online ✅ Vibration monitoring started");
}

// ==================================================================
void loop() {
  Blynk.run();

  // ---- Accelerometer ----
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);
  float accX = ax / 16384.0;
  float accY = ay / 16384.0;
  float accZ = az / 16384.0;

  // ---- Gyroscope ----
  int16_t gx_raw, gy_raw, gz_raw;
  mpu.getRotation(&gx_raw, &gy_raw, &gz_raw);
  float gyroX = gx_raw / 131.0; // °/s
  float gyroY = gy_raw / 131.0;
  float gyroZ = gz_raw / 131.0;

  // ---- Telegram Vibration Detection ----
  float magnitude = sqrt(accX*accX + accY*accY + accZ*accZ);
  float net = fabs(magnitude - 1.0);
  if (net > threshold) {
    unsigned long now = millis();
    if (now - lastAlert > alertCooldown) {
      lastAlert = now;
      String msg = "⚠️ ABNORMAL Vibration Detected! Net Accel = " + String(net, 2) + " g";
      sendTelegram(msg);
    }
  }

  // ---- Debug Serial ----
  Serial.printf("Acc[g] X=%.2f Y=%.2f Z=%.2f | Gyro[°/s] X=%.2f Y=%.2f Z=%.2f\n",
                accX, accY, accZ, gyroX, gyroY, gyroZ);

  // ---- Send to Blynk ----
  Blynk.virtualWrite(V0, accX);
  Blynk.virtualWrite(V1, accY);
  Blynk.virtualWrite(V2, accZ);
  Blynk.virtualWrite(V3, gyroX);
  Blynk.virtualWrite(V4, gyroY);
  Blynk.virtualWrite(V5, gyroZ);

  delay(10);
}
