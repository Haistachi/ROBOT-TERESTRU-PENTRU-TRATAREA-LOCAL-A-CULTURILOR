#pragma once

#include <Arduino.h>

//comunicarea seriala
constexpr uint32_t SERIAL_BAUD = 9600;
constexpr unsigned long COMMUNICATION_TIMEOUT_MS = 2000;
//motoare
constexpr uint8_t PIN_MOTOR_L_DIR = 2;
constexpr uint8_t PIN_MOTOR_R_DIR = 4;
constexpr uint8_t PIN_MOTOR_L_PWM = 5;
constexpr uint8_t PIN_MOTOR_R_PWM = 6;
//senzori
constexpr uint8_t PIN_IR_LEFT = A1;
constexpr uint8_t PIN_IR_RIGHT = A2;
constexpr uint8_t PIN_US_TRIG = 12;
constexpr uint8_t PIN_US_ECHO = 13;
constexpr uint8_t PIN_US_SERVO = 9;
constexpr bool IR_OBSTACLE_LEVEL = LOW;
//pompa
constexpr uint8_t PIN_PUMP = 10;
constexpr bool PUMP_ACTIVE_HIGH = false;
//tun
constexpr uint8_t PIN_TURRET_H = 11;
constexpr uint8_t PIN_TURRET_V = 7;

//viteze si limite
constexpr int TURRET_STOP = 90;
constexpr int TURRET_SLOW_CW = 85;
constexpr int TURRET_SLOW_CCW = 97;
constexpr int TURRET_FAST_CW = 82;
constexpr int TURRET_FAST_CCW = 100;
constexpr float TURRET_LIMIT_DEG = 180.0f;
//pozitii
constexpr float TURRET_SLOW_DEG_PER_SEC = 25.0f;
constexpr float TURRET_FAST_DEG_PER_SEC = 50.0f;