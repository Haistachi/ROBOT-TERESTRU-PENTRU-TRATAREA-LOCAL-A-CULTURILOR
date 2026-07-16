#include "NetworkManager.h"
#include <Arduino.h>
#include <WiFi.h>
#include "Config.h"

void connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("[WIFI] Conectare");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }
  Serial.println();
  Serial.println("[WIFI] Conectat");
  Serial.print("[WIFI] Adresa IP: ");
  Serial.println(WiFi.localIP());
}

void maintainWifiConnection() {
  static unsigned long lastReconnectAttempt = 0;
  if (WiFi.status() == WL_CONNECTED) { return; }
  if (millis() - lastReconnectAttempt < 5000) { return; }
  lastReconnectAttempt = millis();
  Serial.println("[WIFI] Conexiune pierduta");
  Serial.println("[WIFI] Incercare reconectare...");
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void printNetworkInfo() {
  Serial.println();
  Serial.println("--- Informatii retea ---");
  Serial.print("[WIFI] IP:");
  Serial.println(WiFi.localIP());
  Serial.print("[WIFI] RSSI:");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
  Serial.print("[HTTP] Control: http:");
  Serial.println(WiFi.localIP());
  Serial.print("[HTTP] Stream: http:");
  Serial.print(WiFi.localIP());
  Serial.println(":81/stream");
  Serial.println("-------------------------------");
}