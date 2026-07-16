#pragma once

#include <Arduino.h>

// Initializarea senzorilor IR, ultrasonic si servo radar
void initSenzori();

// Masurarea distantei cu HC-SR04
float readDistanceCm();

// Citirea senzorilor IR
bool readIrLeft();
bool readIrRight();

// Controlul servoului pe care este montat senzorul ultrasonic
void setRadarAngle(int angle);
int getRadarAngle();