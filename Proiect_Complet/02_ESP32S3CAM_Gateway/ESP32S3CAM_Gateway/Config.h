#pragma once

#include <Arduino.h>

// wifi
inline constexpr const char* WIFI_SSID = "DIGI-7a7Z";
inline constexpr const char* WIFI_PASSWORD = "Xcs9AsaeXY";

// comunicare arduino
// GPIO19 = TX către RX arduino
// GPIO20 = RX din TX arduino
inline constexpr int ARDUINO_RX_PIN = 20;
inline constexpr int ARDUINO_TX_PIN = 19;
inline constexpr uint32_t ARDUINO_BAUD = 9600;
inline constexpr uint32_t RESPONSE_TIMEOUT_MS = 1000;

// camera
#define PWDN_GPIO_NUM  -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  15
#define SIOD_GPIO_NUM   4
#define SIOC_GPIO_NUM   5
#define Y9_GPIO_NUM    16
#define Y8_GPIO_NUM    17
#define Y7_GPIO_NUM    18
#define Y6_GPIO_NUM    12
#define Y5_GPIO_NUM    10
#define Y4_GPIO_NUM     8
#define Y3_GPIO_NUM     9
#define Y2_GPIO_NUM    11
#define VSYNC_GPIO_NUM  6
#define HREF_GPIO_NUM   7
#define PCLK_GPIO_NUM  13