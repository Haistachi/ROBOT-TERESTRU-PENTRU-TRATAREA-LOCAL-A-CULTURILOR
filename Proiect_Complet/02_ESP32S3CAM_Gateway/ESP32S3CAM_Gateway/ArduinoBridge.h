#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

// Inițializează comunicația UART cu Arduino Uno
void initArduinoBridge();

// Trimite un cadru către Arduino și așteaptă răspunsul
String transactArduino(const String& frame);

// Extrage valoarea unei chei dintr-un răspuns Arduino
// Exemplu: "SENSORS,US=25,IRL=1" + "US" -> "25"
String valueOf(
  const String& payload,
  const String& key
);

// Transformă configurația unui motor într-o valoare semnată
// forward = pozitiv, backward = negativ, stop = 0
int signedMotor(JsonVariantConst motor);