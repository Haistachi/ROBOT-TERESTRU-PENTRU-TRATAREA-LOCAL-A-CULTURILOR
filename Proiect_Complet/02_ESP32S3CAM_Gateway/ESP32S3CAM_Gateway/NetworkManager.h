#pragma once

// Conectează ESP32-S3-CAM la rețeaua Wi-Fi
void connectWifi();

// Verifică periodic conexiunea și încearcă reconectarea
void maintainWifiConnection();

// Afișează în Serial Monitor informațiile de acces
void printNetworkInfo();