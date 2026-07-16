#pragma once

#include <Arduino.h>

// Inițializează servourile tunului
void initTun();

// Trimite o comandă către una dintre axe:
// axis = 'H' orizontal sau 'V' vertical
// command = 82-repede, 85-incet, 90-stop, 97+incet, 100+repede
bool setTurretCommand(char axis, int command);

// Oprește ambele servouri
void stopTurret();

// Actualizează pozițiile estimate
void updateTurretPosition();

// Returnează pozițiile estimate
float getTurretHorizontalPosition();
float getTurretVerticalPosition();

// Returnează comenzile curente
int getTurretHorizontalCommand();
int getTurretVerticalCommand();