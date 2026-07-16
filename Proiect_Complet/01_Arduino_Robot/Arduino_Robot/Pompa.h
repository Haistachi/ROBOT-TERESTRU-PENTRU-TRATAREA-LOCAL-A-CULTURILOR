#pragma once

#include <Arduino.h>

// Inițializează pinul pompei și o lasă oprită
void initPompa();

// Pornește pompa
void pumpOn();

// Oprește pompa
void pumpOff();

// Setează starea pompei
void setPumpState(bool enabled);

// Returnează starea logică a pompei
bool isPumpOn();