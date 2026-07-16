#include "PiniArduino.h"

#include "Miscare.h"
#include "Senzori.h"
#include "Tun.h"
#include "Pompa.h"
#include "SerialProtocol.h"

void setup() {

  Serial.begin(SERIAL_BAUD);

  initMiscare();
  initSenzori();
  initTun();
  initPompa();

  stopAll();

  Serial.println(F("<READY>"));
}

void loop() {

  handleSerialProtocol();

  updateTurretPosition();

  checkCommunicationWatchdog();
}