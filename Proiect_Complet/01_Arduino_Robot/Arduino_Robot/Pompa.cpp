#include "Pompa.h"
#include "PiniArduino.h"

static bool pumpState = false;

static uint8_t pumpOutputLevel(bool enabled) {

  if (PUMP_ACTIVE_HIGH) 
  { return enabled ? HIGH : LOW;}
  return enabled ? LOW : HIGH;
}

void initPompa() {
  pinMode(PIN_PUMP,OUTPUT);
  pumpOff();
}

void setPumpState(bool enabled) {
  digitalWrite(PIN_PUMP,pumpOutputLevel(enabled));
  pumpState = enabled;
}

void pumpOn() {setPumpState(true);}
void pumpOff() {setPumpState(false);}
bool isPumpOn() {return pumpState;}