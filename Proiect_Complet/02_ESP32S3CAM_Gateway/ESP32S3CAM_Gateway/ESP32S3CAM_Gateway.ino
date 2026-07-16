#include "Config.h"
#include "CameraModule.h"
#include "ArduinoBridge.h"
#include "ApiRoutes.h"
#include "NetworkManager.h"

void setup() {
  Serial.begin(115200);
  delay(500);

  initArduinoBridge();
  cameraReady = initCamera();
  connectWifi();
  registerRoutes();
  startControlServer();

  if (cameraReady) {
    startStreamServer();
  }

  printNetworkInfo();
}

void loop() {
  handleControlServer();
  maintainWifiConnection();
  delay(2);
}