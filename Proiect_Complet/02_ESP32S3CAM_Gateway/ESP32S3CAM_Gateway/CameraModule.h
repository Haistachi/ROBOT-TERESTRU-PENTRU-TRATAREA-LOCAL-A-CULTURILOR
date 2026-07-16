#pragma once

#include <Arduino.h>

// Indică dacă inițializarea camerei a reușit
extern bool cameraReady;

// Inițializează camera OV2640
bool initCamera();

// Pornește serverul de streaming video
bool startStreamServer();