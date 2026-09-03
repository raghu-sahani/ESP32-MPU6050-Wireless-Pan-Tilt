#include <WiFi.h>
#include <esp_now.h>
#include <ESP32Servo.h>

#define PAN_SERVO_PIN 18
#define TILT_SERVO_PIN 19

Servo panServo;
Servo tiltServo;

typedef struct struct_message {
  int pan;
  int tilt;
} struct_message;

struct_message incomingData;

void OnDataRecv(const uint8_t *mac,
                const uint8_t *incomingDataBytes,
                int len) {
  if (len != sizeof(incomingData)) {
    return;
  }

  memcpy(&incomingData, incomingDataBytes, sizeof(incomingData));

  int panAngle = constrain(incomingData.pan, 0, 180);
  int tiltAngle = constrain(incomingData.tilt, 0, 180);

  panServo.write(panAngle);
  tiltServo.write(tiltAngle);

  Serial.print("PAN: ");
  Serial.print(panAngle);
  Serial.print(" | TILT: ");
  Serial.println(tiltAngle);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("================================");
  Serial.println("ESP32 PAN/TILT RECEIVER");
  Serial.println("================================");

  panServo.setPeriodHertz(50);
  tiltServo.setPeriodHertz(50);

  panServo.attach(PAN_SERVO_PIN, 500, 2400);
  tiltServo.attach(TILT_SERVO_PIN, 500, 2400);

  panServo.write(90);
  tiltServo.write(90);

  delay(500);

  WiFi.mode(WIFI_STA);

  Serial.print("Receiver MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW INIT FAILED");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("ESP-NOW READY");
  Serial.println("Waiting for sender...");
}

void loop() {
  delay(20);
}
