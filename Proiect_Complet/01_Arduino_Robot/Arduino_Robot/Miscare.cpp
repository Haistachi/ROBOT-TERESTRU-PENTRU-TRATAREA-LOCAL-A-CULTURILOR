#include "Miscare.h" 
#include "PiniArduino.h" 

static unsigned long lastMotionCommandMs = 0; 
static bool motorsRunning = false; 
void initMiscare() 
{ 
  pinMode(PIN_MOTOR_L_DIR, OUTPUT); 
  pinMode(PIN_MOTOR_R_DIR, OUTPUT); 
  pinMode(PIN_MOTOR_L_PWM, OUTPUT); 
  pinMode(PIN_MOTOR_R_PWM, OUTPUT); 
  stopMovement(); 
} 

void setLeftMotor(int speed) 
{ 
  speed = constrain(speed, -255, 255); 
  if (speed > 0) 
  { 
    digitalWrite( PIN_MOTOR_L_DIR, HIGH ); 
    analogWrite( PIN_MOTOR_L_PWM, speed ); 
  } else if (speed < 0) 
  { 
    digitalWrite( PIN_MOTOR_L_DIR, LOW ); 
    analogWrite( PIN_MOTOR_L_PWM, -speed ); 
  } else { 
    analogWrite( PIN_MOTOR_L_PWM, 0 ); 
  } 
} 

void setRightMotor(int speed) 
{ 
  speed = constrain(speed, -255, 255); 
  if (speed > 0) 
  { 
    digitalWrite( PIN_MOTOR_R_DIR, HIGH ); 
    analogWrite( PIN_MOTOR_R_PWM, speed ); 
  } else if (speed < 0) 
  { 
    digitalWrite( PIN_MOTOR_R_DIR, LOW ); 
    analogWrite( PIN_MOTOR_R_PWM, -speed ); 
  } else { 
    analogWrite( PIN_MOTOR_R_PWM, 0 ); 
  } 
} 

void driveMotors( int leftSpeed, int rightSpeed ) 
{ 
  setLeftMotor(leftSpeed); 
  setRightMotor(rightSpeed); 
  motorsRunning = leftSpeed != 0 || rightSpeed != 0; 
  registerMotionCommand(); 
} 

void forward(int speed) 
{ 
  speed = constrain( speed, 0, 255 ); 
  driveMotors( speed, speed ); 
} 

void backward(int speed) 
{ 
  speed = constrain( speed, 0, 255 ); 
  driveMotors( -speed, -speed ); 
} 

void turnLeft(int speed) 
{ 
  speed = constrain( speed, 0, 255 ); 
  driveMotors( 0, speed ); 
} 

void turnRight(int speed) 
{ 
  speed = constrain( speed, 0, 255 ); 
  driveMotors( speed, 0 ); 
} 

void rotateLeft(int speed) 
{ 
  speed = constrain( speed, 0, 255 ); 
  driveMotors( -speed, speed ); 
} 

void rotateRight(int speed) 
{ 
  speed = constrain( speed, 0, 255 ); 
  driveMotors( speed, -speed ); 
} 

void stopMovement() 
{ 
  setLeftMotor(0); 
  setRightMotor(0); 
  motorsRunning = false; 
} 

bool areMotorsRunning() { return motorsRunning; } 

void registerMotionCommand() 
{ 
  lastMotionCommandMs = millis(); 
} 

void updateMotionWatchdog() 
{ 
  if (!motorsRunning) { return; } 
  if ( millis() - lastMotionCommandMs > COMMUNICATION_TIMEOUT_MS ) 
  { stopMovement(); } 
}