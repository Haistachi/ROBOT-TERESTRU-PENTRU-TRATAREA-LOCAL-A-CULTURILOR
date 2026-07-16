#pragma once

#include <Arduino.h>

// Inițializarea pinilor pentru motoare
void initMiscare();

// Control individual pentru fiecare motor
void setLeftMotor(int speed);
void setRightMotor(int speed);

// Control simultan
void driveMotors(int leftSpeed, int rightSpeed);

// Mișcări standard
void forward(int speed);
void backward(int speed);
void turnLeft(int speed);
void turnRight(int speed);
void rotateLeft(int speed);
void rotateRight(int speed);

// Oprire
void stopMovement();

// Stare
bool areMotorsRunning();

// Watchdog pentru oprire automată
void updateMotionWatchdog();
void registerMotionCommand();