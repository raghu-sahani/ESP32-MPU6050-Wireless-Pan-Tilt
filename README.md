# ESP32 MPU6050 Wireless Pan-Tilt

Wireless pan/tilt control using two ESP32 boards and ESP-NOW.

## Project Overview

- Sender: ESP32 + MPU6050
- Receiver: ESP32 + 2 servo motors
- Wireless communication: ESP-NOW
- Pan control: Yaw using MPU6050 gyro Z-axis
- Tilt control: Roll using MPU6050 accelerometer
- ESP32 Arduino Core: tested for 2.0.10 callback format

## Sender Connections

| MPU6050 | ESP32 Sender |
|---|---|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

## Receiver Connections

| Device | ESP32 Receiver |
|---|---|
| Pan servo signal | GPIO 18 |
| Tilt servo signal | GPIO 19 |
| Servo VCC | External 5V supply |
| Servo GND | External supply GND + ESP32 GND |

Important: use a common ground between the receiver ESP32 and the external servo power supply. Do not power both servos from the ESP32 3.3V pin.

## Receiver MAC Address

Current receiver MAC used by the sender:

`00:70:07:E3:15:D8`

In the sender sketch it is written as:

```cpp
uint8_t receiverAddress[] = {
  0x00, 0x70, 0x07, 0xE3, 0x15, 0xD8
};
```

If you replace the receiver ESP32, update this MAC address in the sender code.

## Required Arduino Libraries

Install these libraries using Arduino IDE Library Manager:

- Adafruit MPU6050
- Adafruit Unified Sensor
- Adafruit BusIO
- ESP32Servo

`WiFi.h`, `esp_now.h`, and `Wire.h` are included with the ESP32 Arduino core.

## How It Works

The sender reads MPU6050 motion data. The Z-axis gyroscope is integrated to estimate yaw and control the pan servo. Roll is calculated from accelerometer Y/Z data and controls the tilt servo. The resulting pan and tilt angles are transmitted wirelessly using ESP-NOW.

The receiver listens for the two angle values and directly updates the two servo motors.

## Servo Pins

```text
Pan servo  -> GPIO 18
Tilt servo -> GPIO 19
```

Both servos start at 90 degrees.

## Sensitivity

The sender currently uses:

```cpp
float smoothing = 0.25;
```

A larger value gives a faster response. A smaller value gives smoother but slower movement.

## Note About Yaw

The MPU6050 does not contain a magnetometer, so gyro-based yaw will slowly drift over time. For long-term absolute heading, a sensor such as BNO055, BNO085, or an IMU with a magnetometer is more suitable.
