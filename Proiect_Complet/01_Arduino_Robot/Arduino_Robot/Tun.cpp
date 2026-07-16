#include "Tun.h" 
#include "PiniArduino.h" 
#include <Servo.h> 

static Servo turretH; 
static Servo turretV; 
static float turretHorizontalPosition = 0.0f; 
static float turretVerticalPosition = 0.0f; 
static int turretHorizontalCommand = TURRET_STOP; 
static int turretVerticalCommand = TURRET_STOP; 
static unsigned long lastTurretUpdateMs = 0; 

static bool isValidTurretCommand(int command) 
{
  return command == TURRET_FAST_CW || 
  command == TURRET_SLOW_CW || command == TURRET_STOP || 
  command == TURRET_SLOW_CCW || command == TURRET_FAST_CCW; 
} 

static float commandToSpeed(int command) 
{ 
  switch (command) 
  { 
    case TURRET_FAST_CW: 
      return -TURRET_FAST_DEG_PER_SEC; 
    case TURRET_SLOW_CW: 
      return -TURRET_SLOW_DEG_PER_SEC; 
    case TURRET_STOP: 
      return 0.0f; 
    case TURRET_SLOW_CCW: 
      return TURRET_SLOW_DEG_PER_SEC; 
    case TURRET_FAST_CCW: 
      return TURRET_FAST_DEG_PER_SEC; 
    default: 
      return 0.0f; 
  } 
} 

void initTun() 
{ 
  turretH.attach(PIN_TURRET_H); 
  turretV.attach(PIN_TURRET_V); 
  turretHorizontalPosition = 0.0f; 
  turretVerticalPosition = 0.0f; 
  turretHorizontalCommand = TURRET_STOP; 
  turretVerticalCommand = TURRET_STOP; 
  turretH.write(TURRET_STOP); 
  turretV.write(TURRET_STOP); 
  lastTurretUpdateMs = millis(); 
} 

void updateTurretPosition() 
{ 
  const unsigned long now = millis(); 
  const float deltaTime = (now - lastTurretUpdateMs) / 1000.0f; 
  lastTurretUpdateMs = now; 
  const float horizontalSpeed = commandToSpeed( turretHorizontalCommand ); 
  turretHorizontalPosition += horizontalSpeed * deltaTime; 
  
  if ( turretHorizontalPosition <= -TURRET_LIMIT_DEG ) 
  { 
    turretHorizontalPosition = -TURRET_LIMIT_DEG; 
    turretHorizontalCommand = TURRET_STOP; 
    turretH.write( TURRET_STOP ); 
  } 
  if ( turretHorizontalPosition >= TURRET_LIMIT_DEG ) 
  { 
    turretHorizontalPosition = TURRET_LIMIT_DEG; 
    turretHorizontalCommand = TURRET_STOP; 
    turretH.write( TURRET_STOP ); 
  } 
  
  const float verticalSpeed = commandToSpeed( turretVerticalCommand ); 
  turretVerticalPosition += verticalSpeed * deltaTime; 
  
  if ( turretVerticalPosition <= -TURRET_LIMIT_DEG ) 
  { 
    turretVerticalPosition = -TURRET_LIMIT_DEG; 
    turretVerticalCommand = TURRET_STOP; 
    turretV.write( TURRET_STOP ); 
  } 
  if ( turretVerticalPosition >= TURRET_LIMIT_DEG ) 
  { 
    turretVerticalPosition = TURRET_LIMIT_DEG; 
    turretVerticalCommand = TURRET_STOP; 
    turretV.write( TURRET_STOP ); 
  } 
} 

bool setTurretCommand( char axis, int command ) 
{ 
  //if ( !isValidTurretCommand( command ) ) { return false; } 
  //-scos pentru flexibilitate
  updateTurretPosition(); 
  if ( axis == 'H' ) 
  { 
    if ( turretHorizontalPosition <= -TURRET_LIMIT_DEG &&
     commandToSpeed(command) < 0 ) 
    { 
      turretHorizontalCommand = TURRET_STOP; 
      turretH.write( TURRET_STOP ); 
      return true; 
    } 

    if ( turretHorizontalPosition >= TURRET_LIMIT_DEG && 
      commandToSpeed(command) > 0 ) 
    { 
      turretHorizontalCommand = TURRET_STOP; 
      turretH.write( TURRET_STOP ); 
      return true; 
    } 
    
    turretHorizontalCommand = command; 
    turretH.write( command ); 
    return true; 
  } 
  
  if ( axis == 'V' ) 
  { 
    if ( turretVerticalPosition <= -TURRET_LIMIT_DEG &&
     commandToSpeed(command) < 0 ) 
    { 
      turretVerticalCommand = TURRET_STOP; 
      turretV.write( TURRET_STOP ); 
      return true; 
    } 
    
    if ( turretVerticalPosition >= TURRET_LIMIT_DEG && 
    commandToSpeed(command) > 0 ) 
    { 
      turretVerticalCommand = TURRET_STOP; 
      turretV.write( TURRET_STOP ); 
      return true; 
    } 
    
    turretVerticalCommand = command; 
    turretV.write( command ); 
    return true; 
  } 
  
  return false; 
} 

void stopTurret() 
{ 
  turretHorizontalCommand = TURRET_STOP; 
  turretVerticalCommand = TURRET_STOP; 
  turretH.write( TURRET_STOP ); 
  turretV.write( TURRET_STOP ); 
} 

float getTurretHorizontalPosition() 
{ return turretHorizontalPosition; } 

float getTurretVerticalPosition() 
{ return turretVerticalPosition; } 

int getTurretHorizontalCommand() 
{ return turretHorizontalCommand; } 

int getTurretVerticalCommand() 
{ return turretVerticalCommand; }