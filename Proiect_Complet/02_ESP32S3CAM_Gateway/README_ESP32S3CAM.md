# ESP32-S3-CAM Gateway + OV2640

Firmware-ul din `ESP32S3CAM_Gateway.ino` înlocuiește firmware-ul pentru vechiul ESP32-CAM AI Thinker.

## Conexiune UART cu Arduino UNO

- ESP32-S3-CAM GPIO19 (TX1) -> Arduino UNO D0/RX
- Arduino UNO D1/TX -> divizor de tensiune / convertor logic 5 V -> 3.3 V -> ESP32-S3-CAM GPIO20 (RX1)
- GND ESP32-S3-CAM <-> GND Arduino UNO

Pinii RX/TX marcați pe placa ESP32-S3-CAM rămân disponibili pentru programare și monitorul serial.

## Arduino IDE

Selectează `ESP32S3 Dev Module`.
Setări recomandate pentru această placă:
- CPU Frequency: 240 MHz
- Flash Mode: DIO 80 MHz
- Flash Size: 16 MB
- PSRAM: OPI PSRAM
- Upload Mode: UART0 / Hardware CDC
- USB CDC on Boot: Disabled

La prima programare poate fi necesar `Erase All Flash Before Sketch Upload = Enabled`; după prima încărcare se revine la `Disabled`.

## Biblioteci

- WiFi (din Arduino-ESP32)
- WebServer (din Arduino-ESP32)
- ArduinoJson
- esp_camera (din Arduino-ESP32)
- esp_http_server (ESP-IDF inclus prin Arduino-ESP32)

## Adrese

- Control/status: `http://IP_ESP32/`
- Status: `http://IP_ESP32/status`
- Senzori: `http://IP_ESP32/sensors`
- Stream MJPEG: `http://IP_ESP32:81/stream`
