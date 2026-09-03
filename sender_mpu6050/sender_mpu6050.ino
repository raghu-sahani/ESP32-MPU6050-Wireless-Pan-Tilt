#include <WiFi.h>
#include <esp_now.h>

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

uint8_t receiverAddress[] = {
  0x00, 0x70, 0x07, 0xE3, 0x15, 0xD8
};

typedef struct struct_message {
  int pan;
  int tilt;
} struct_message;

struct_message sendData;
esp_now_peer_info_t peerInfo;

float smoothPan = 90.0;
float smoothTilt = 90.0;
float smoothing = 0.25;

float yaw = 0.0;
unsigned long lastTime = 0;
float gyroDeadZone = 1.0;

void OnDataSent(const uint8_t *mac_addr,
                esp_now_send_status_t status) {
  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.print(" | SEND OK");
  } else {
    Serial.print(" | SEND FAILED");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("====================================");
  Serial.println("ESP32 MPU6050 SENDER");
  Serial.println("PAN  = YAW");
  Serial.println("TILT = ROLL");
  Serial.println("====================================");

  Wire.begin(21, 22);

  if (!mpu.begin()) {
    Serial.println("MPU6050 NOT FOUND!");
    while (1) {
      delay(1000);
    }
  }

  Serial.println("MPU6050 CONNECTED");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  WiFi.mode(WIFI_STA);

  Serial.print("Sender MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW INIT FAILED");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("FAILED TO ADD RECEIVER");
    return;
  }

  Serial.println("ESP-NOW READY");

  lastTime = millis();
  yaw = 0;
  smoothPan = 90;
  smoothTilt = 90;
  sendData.pan = 90;
  sendData.tilt = 90;

  Serial.println("SYSTEM READY");
}

void loop() {
  sensors_event_t a;
  sensors_event_t g;
  sensors_event_t temp;

  mpu.getEvent(&a, &g, &temp);

  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0;
  lastTime = now;

  if (dt <= 0 || dt > 0.2) {
    dt = 0.02;
  }

  float gyroZ = g.gyro.z * 180.0 / PI;

  if (abs(gyroZ) < gyroDeadZone) {
    gyroZ = 0;
  }

  yaw += gyroZ * dt;
  yaw = constrain(yaw, -90.0, 90.0);

  int targetPan = map(
    (int)yaw,
    -90,
    90,
    0,
    180
  );

  float roll = atan2(
    a.acceleration.y,
    a.acceleration.z
  ) * 180.0 / PI;

  roll = constrain(roll, -60.0, 60.0);

  int targetTilt = map(
    (int)roll,
    -60,
    60,
    0,
    180
  );

  smoothPan = smoothPan + smoothing * (targetPan - smoothPan);
  smoothTilt = smoothTilt + smoothing * (targetTilt - smoothTilt);

  sendData.pan = constrain((int)smoothPan, 0, 180);
  sendData.tilt = constrain((int)smoothTilt, 0, 180);

  esp_now_send(
    receiverAddress,
    (uint8_t *)&sendData,
    sizeof(sendData)
  );

  Serial.print("Yaw: ");
  Serial.print(yaw, 1);
  Serial.print(" | GyroZ: ");
  Serial.print(gyroZ, 1);
  Serial.print(" | PAN: ");
  Serial.print(sendData.pan);
  Serial.print(" | Roll: ");
  Serial.print(roll, 1);
  Serial.print(" | TILT: ");
  Serial.print(sendData.tilt);
  Serial.println();

  delay(20);
}
