#include "ArduinoBridge.h"
#include "Config.h"

// UART1 este utilizat pentru comunicația cu Arduino Uno
HardwareSerial ArduinoSerial(1);


void initArduinoBridge() {
  ArduinoSerial.begin(
    ARDUINO_BAUD,
    SERIAL_8N1,
    ARDUINO_RX_PIN,
    ARDUINO_TX_PIN
  );

  Serial.printf(
    "[UART] Arduino: RX=GPIO%d, TX=GPIO%d, %lu baud\n",
    ARDUINO_RX_PIN,
    ARDUINO_TX_PIN,
    static_cast<unsigned long>(ARDUINO_BAUD)
  );
}


String transactArduino(const String& frame) {

  // Elimină eventualele date vechi rămase în buffer
  while (ArduinoSerial.available()) {
    ArduinoSerial.read();
  }

  // Trimite cadrul în format:
  // <COMANDA>
  ArduinoSerial.print('<');
  ArduinoSerial.print(frame);
  ArduinoSerial.print('>');
  ArduinoSerial.flush();

  String response;
  response.reserve(256);

  bool started = false;
  const unsigned long start = millis();

  // Așteaptă răspunsul până la expirarea timeout-ului
  while (millis() - start < RESPONSE_TIMEOUT_MS) {

    while (ArduinoSerial.available()) {

      const char c =
        static_cast<char>(ArduinoSerial.read());

      // Începutul cadrului
      if (c == '<') {
        response = "";
        started = true;
      }

      // Sfârșitul cadrului
      else if (c == '>' && started) {
        return response;
      }

      // Conținutul cadrului
      else if (
        started &&
        response.length() < 255
      ) {
        response += c;
      }
    }

    delay(1);
  }

  // Timeout: Arduino nu a răspuns
  return "";
}


String valueOf(
  const String& payload,
  const String& key
) {
  int start = payload.indexOf(key + "=");

  if (start < 0) {
    return "";
  }

  start += key.length() + 1;

  int end = payload.indexOf(',', start);

  if (end < 0) {
    end = payload.length();
  }

  return payload.substring(start, end);
}


int signedMotor(JsonVariantConst motor) {

  String direction =
    motor["direction"] | "stop";

  direction.toLowerCase();

  const int speed = constrain(
    static_cast<int>(motor["speed"] | 0),
    0,
    255
  );

  if (direction == "backward") {
    return -speed;
  }

  if (direction == "forward") {
    return speed;
  }

  return 0;
}