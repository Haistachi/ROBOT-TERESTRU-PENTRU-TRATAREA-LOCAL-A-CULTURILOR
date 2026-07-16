#include "Senzori.h"
#include "PiniArduino.h"
#include <Servo.h>

static Servo radarServo;
static int radarAngle = 90;

void initSenzori() {

  // Senzori IR
  pinMode(PIN_IR_LEFT, INPUT);
  pinMode(PIN_IR_RIGHT, INPUT);
  // Senzori ultrasonici
  pinMode(PIN_US_TRIG, OUTPUT);
  pinMode(PIN_US_ECHO, INPUT);

  digitalWrite(PIN_US_TRIG, LOW);
  radarServo.attach(PIN_US_SERVO);
  setRadarAngle(90);
}

void setRadarAngle(int angle) {
  radarAngle = constrain(angle,0,180);
  radarServo.write(radarAngle);
}

int getRadarAngle() {  return radarAngle;}

float readDistanceCm() {

  digitalWrite(PIN_US_TRIG,LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_US_TRIG,HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_US_TRIG,LOW);

  const unsigned long duration =
    pulseIn(PIN_US_ECHO,HIGH,30000);

  if (duration == 0) {return -1.0f;}

  // Distanta:
  // viteza sunetului aproximativ 0.0343 cm/us
  // impartire la 2 deoarece semnalul merge dus-intors
  return duration* 0.0343f/ 2.0f;
}

bool readIrLeft() {
  return digitalRead(PIN_IR_LEFT) == IR_OBSTACLE_LEVEL;
}

bool readIrRight() {
  return digitalRead(PIN_IR_RIGHT) == IR_OBSTACLE_LEVEL;
}