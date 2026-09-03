#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  delay(1000);

  Serial.println();
  Serial.println("===========================");
  Serial.println("ESP32 RECEIVER MAC ADDRESS");
  Serial.println("===========================");

  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  Serial.println("===========================");
}

void loop() {
}
