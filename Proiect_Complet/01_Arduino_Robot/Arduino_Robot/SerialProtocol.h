#pragma once

// Procesează comunicația serială
void handleSerialProtocol();

// Verifică watchdog-ul comunicației
void checkCommunicationWatchdog();

// Oprește toate actuatoarele robotului
void stopAll(bool timeout = false);