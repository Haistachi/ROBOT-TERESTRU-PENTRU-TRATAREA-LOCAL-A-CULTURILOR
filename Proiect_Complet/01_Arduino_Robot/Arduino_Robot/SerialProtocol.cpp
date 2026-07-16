#include "SerialProtocol.h" 
#include "Miscare.h" 
#include "Senzori.h" 
#include "Tun.h" 
#include "Pompa.h" 
#include "PiniArduino.h" 

static String frameBuffer; 
static bool receivingFrame = false; 

static String tokenAt( const String& text, int index ) 
{ 
  int tokenStart = 0; 
  int currentIndex = 0; 
  for (int i = 0; i <= text.length(); i++) 
  { 
    const bool tokenEnd = i == text.length() ||
    text.charAt(i) == ','; 

    if (tokenEnd) 
    { 
      if (currentIndex == index) 
      { 
        return text.substring(tokenStart, i); 
      } 
    currentIndex++; tokenStart = i + 1; 
    } 
  } 
  return ""; 
} 
     
static int clampAngle(int angle)
{ return constrain(angle, 0, 180); }

void stopAll(bool timeout) 
{ 
  stopMovement(); 
  stopTurret(); 
  pumpOff(); 
  if (timeout) 
  { Serial.println(F("<WARN,TIMEOUT>")); } 
  return;
} 

void checkCommunicationWatchdog() 
{ updateMotionWatchdog(); return;} 

static void sendSensors() 
{ 
  Serial.print(F("<SENSORS")); 
  Serial.print(F(",US=")); 
  Serial.print(readDistanceCm()); 
  Serial.print(F(",IRL=")); 
  Serial.print(readIrLeft() ? 1 : 0); 
  Serial.print(F(",IRR=")); 
  Serial.print(readIrRight() ? 1 : 0); 
  Serial.print(F(",RADAR=")); 
  Serial.print(getRadarAngle()); 
  Serial.print(F(",TH=")); 
  Serial.print(getTurretHorizontalPosition()); 
  Serial.print(F(",TV=")); 
  Serial.print(getTurretVerticalPosition()); 
  Serial.print(F(",PUMP=")); 
  Serial.print(isPumpOn() ? 1 : 0); 
  Serial.println(F(">"));
  return; 
} 
  
static void processFrame(String frame) 
{ 
  frame.trim(); 
  frame.toUpperCase(); 
  const String cmd = tokenAt(frame, 0); 
  if (cmd.length() == 0) 
  { 
    Serial.println(F("<ERR,EMPTY_COMMAND>")); 
    return; 
  } 
  if (cmd == "PING") 
  { 
    Serial.println(F("<OK,PONG>")); 
    return; 
  } 
  if (cmd == "MOVE") 
  { 
    const String direction = tokenAt(frame, 1); 
    const int speed = constrain( tokenAt(frame, 2).toInt(), 0, 255 ); 
    if (direction == "FWD") 
    { forward(speed); } 
    else if (direction == "BACK") 
    { backward(speed); } 
    else if (direction == "LEFT") 
    { turnLeft(speed); } 
    else if (direction == "RIGHT") 
    { turnRight(speed); } 
    else if (direction == "ROT_L") 
    { rotateLeft(speed); } 
    else if (direction == "ROT_R") 
    { rotateRight(speed); } 
    else if (direction == "STOP") 
    { stopMovement(); } 
    else 
    { Serial.println(F("<ERR,INVALID_MOVE>")); 
    return; 
    } 
    Serial.println(F("<OK,MOVE>")); 
    return; 
  } 
  if (cmd == "DRV") 
  { 
    const int leftSpeed = 
      constrain( tokenAt(frame, 1).toInt(), -255, 255 ); 
    const int rightSpeed = 
      constrain( tokenAt(frame, 2).toInt(), -255, 255 ); 
    driveMotors(leftSpeed, rightSpeed); 
    Serial.println(F("<OK,DRV>")); 
    return; 
  } 
  if (cmd == "PUMP") 
  { 
    const String state = tokenAt(frame, 1); 
    if (state == "ON") 
    { pumpOn(); } 
    else 
    if (state == "OFF") 
    { pumpOff(); } 
    else 
    { Serial.println(F("<ERR,INVALID_PUMP_STATE>")); 
    return; 
    } 
    Serial.println(F("<OK,PUMP>")); 
    return; 
  } 
  if (cmd == "RADAR") 
  { 
    const String angleToken = tokenAt(frame, 1); 
    if (angleToken.length() == 0) 
    { 
      Serial.println(F("<ERR,MISSING_RADAR_ANGLE>")); 
      return; 
    } 
    const int angle = clampAngle( angleToken.toInt() ); 
    setRadarAngle(angle); 
    Serial.print(F("<OK,RADAR,")); 
    Serial.print(getRadarAngle()); 
    Serial.println(F(">")); 
    return; 
  } 
  if (cmd == "TURRET") 
  { 
    const String axisToken = tokenAt(frame, 1); 
    const String valueToken = tokenAt(frame, 2); 
    if ( axisToken.length() == 0 || valueToken.length() ==0 ) 
    { 
      Serial.println(F("<ERR,INVALID_TURRET_FORMAT>")); 
      return; 
    } 
    const char axis = axisToken.charAt(0); 
    const int value = valueToken.toInt(); 
    if (!setTurretCommand(axis, value)) 
    { 
      Serial.println(F("<ERR,INVALID_TURRET_COMMAND>")); 
      return; 
    } 
    Serial.print(F("<OK,TURRET,")); 
    Serial.print(axis); 
    Serial.print(','); 
    Serial.print(value); 
    Serial.println(F(">")); 
    return; 
  } 
  if (cmd == "REQ") 
  { 
    const String request = tokenAt(frame, 1); 
    if (request == "SENSORS") 
    { sendSensors(); return; } 
    Serial.println(F("<ERR,INVALID_REQUEST>")); 
    return; 
  } 
  if (cmd == "STOP") 
  { 
    stopAll(); 
    Serial.println(F("<OK,STOP>")); 
    return; 
  } 
  Serial.println(F("<ERR,UNKNOWN_COMMAND>")); 
} 

void handleSerialProtocol() 
{ 
  while (Serial.available()) 
  { 
    const char c = 
      static_cast<char>(Serial.read());

    if (c == '<') 
    { 
      frameBuffer = ""; 
      receivingFrame = true; 
      continue; 
    }

    if (!receivingFrame) 
    { continue; } 

    if (c == '>') 
    { 
      receivingFrame = false; 
      if (frameBuffer.length() > 0) 
      { 
        processFrame(frameBuffer); 
      } else { 
        Serial.println(F("<ERR,EMPTY_FRAME>")); 
      } 
      frameBuffer = ""; 
      continue; 
    } 
    if (c == '\r' || c == '\n') { continue; }
     
    if (frameBuffer.length() < 95) 
    { frameBuffer += c; } 
    else 
    { 
      frameBuffer = ""; 
      receivingFrame = false; 
      Serial.println( F("<ERR,BUFFER_OVERFLOW>") ); 
    } 
  } 
}