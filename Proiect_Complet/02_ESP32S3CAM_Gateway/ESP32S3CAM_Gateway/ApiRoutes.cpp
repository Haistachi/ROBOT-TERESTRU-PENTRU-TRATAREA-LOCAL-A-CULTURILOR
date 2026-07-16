#include "ApiRoutes.h"
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "ArduinoBridge.h"
#include "CameraModule.h"
// SERVER HTTP PRINCIPAL
static WebServer server(80);

// FUNCTII AUXILIARE HTTP / JSON
static void addCors() {
  server.sendHeader(
    "Access-Control-Allow-Origin",
    "*");
  server.sendHeader(
    "Access-Control-Allow-Headers",
    "Content-Type");
  server.sendHeader(
    "Access-Control-Allow-Methods",
    "GET,POST,OPTIONS");
}
static void sendJson(
  int code,
  const String& json) {
  addCors();
  server.send(
    code,
    "application/json",
    json);
}
static void handleOptions() {
  addCors();
  server.send(204);
}
static bool readBody(
  JsonDocument& doc) {
  if (!server.hasArg("plain")) {
    return false;
  }
  return deserializeJson(
           doc,
           server.arg("plain"))
         == DeserializationError::Ok;
}
static String escapeJson(
  String value) {
  value.replace("\\", "\\\\");
  value.replace("\"", "\\\"");
  value.replace("\r", "\\r");
  value.replace("\n", "\\n");
  return value;
}

// conversie raspuns ARDUINO -> HTTP
static void sendArduinoResult(
  const String& reply) {
  if (reply.isEmpty()) {
    sendJson(
      504,
      "{\"success\":false,"
      "\"error\":\"Arduino nu raspunde\"}");
    return;
  }
  const bool ok =
    reply.startsWith("OK,");
  const String json =
    String("{\"success\":")
    + (ok ? "true" : "false")
    + ",\"arduino_reply\":\""
    + escapeJson(reply)
    + "\"}";
  sendJson(
    ok ? 200 : 400,
    json);
}

static void handleMove() {
  StaticJsonDocument<192> doc;
  if (!readBody(doc)) {
    sendJson(
      400,
      "{\"success\":false,"
      "\"error\":\"JSON invalid\"}");
    return;
  }
  String direction =
    doc["direction"] | "stop";
  direction.toUpperCase();
  const int speed =
    constrain(
      static_cast<int>(
        doc["speed"] | 150),
      0,
      255);
  const String reply =
    transactArduino(
      "MOVE,"
      + direction
      + ","
      + String(speed));
  sendArduinoResult(reply);
}

static void handleMotor() {
  StaticJsonDocument<256> doc;
  if (!readBody(doc)) {
    sendJson(
      400,
      "{\"success\":false,"
      "\"error\":\"JSON invalid\"}");
    return;
  }
  const int left =
    signedMotor(
      doc["left_motor"]);
  const int right =
    signedMotor(
      doc["right_motor"]);
  const String reply =
    transactArduino(
      "DRV,"
      + String(left)
      + ","
      + String(right));
  sendArduinoResult(reply);
}

static void handlePump() {
  StaticJsonDocument<96> doc;
  if (!readBody(doc)) {
    sendJson(
      400,
      "{\"success\":false,"
      "\"error\":\"JSON invalid\"}");
    return;
  }
  String state =
    doc["pump"] | "off";
  state.toUpperCase();
  const String reply =
    transactArduino(
      "PUMP,"
      + state);
  sendArduinoResult(reply);
}

static void handleRadar() {
  StaticJsonDocument<96> doc;
  if (!readBody(doc)) {
    sendJson(
      400,
      "{\"success\":false,"
      "\"error\":\"JSON invalid\"}");
    return;
  }
  const int angle =
    constrain(
      static_cast<int>(
        doc["angle"] | 90),
      0,
      180);
  const String reply =
    transactArduino(
      "RADAR,"
      + String(angle));
  sendArduinoResult(reply);
}

static void handleTurret() {
  StaticJsonDocument<128> doc;
  if (!readBody(doc)) {
    sendJson(
      400,
      "{\"success\":false,"
      "\"error\":\"JSON invalid\"}");
    return;
  }
  String axis =
    doc["axis"] | "";
  axis.toUpperCase();
  const int command =
    static_cast<int>(
      doc["command"] | 90);
  const bool validCommand =
    command == 82
    || command == 85
    || command == 90
    || command == 97
    || command == 100;
  if (
    (axis != "H" && axis != "V")
    || !validCommand) {
    sendJson(
      400,
      "{\"success\":false,"
      "\"error\":\"Comanda turela invalida\"}");
    return;
  }
  const String reply =
    transactArduino(
      "TURRET,"
      + axis
      + ","
      + String(command));
  sendArduinoResult(reply);
}

static void handleSensors() {
  const String reply =
    transactArduino(
      "REQ,SENSORS");
  if (
    !reply.startsWith(
      "SENSORS,")) {
    sendJson(
      504,
      "{\"success\":false,"
      "\"error\":\"Raspuns senzori invalid\"}");
    return;
  }
  const String us =
    valueOf(
      reply,"US");
  const String radar =valueOf(reply,"RADAR");
  const String th = valueOf(reply,"TH");
  const String tv = valueOf(reply,"TV");
  String json = "{\"success\":true";
  json +=",\"ultrasonic_cm\":"+ (us.length() ? us : "null");
  json += ",\"ir_left\":" + String( valueOf( reply,"IRL")   == "1"? "true" : "false");
  json +=",\"ir_right\":" + String( valueOf( reply, "IRR")  == "1" ? "true" : "false");
  json += ",\"pump\":" + String(valueOf( reply, "PUMP")  == "1" ? "true" : "false");
  json += ",\"radar_angle\":" + (radar.length() ? radar : "null");
  json += ",\"turret_horizontal\":" + (th.length() ? th : "null");
  json += ",\"turret_vertical\":" + (tv.length() ? tv : "null");json += "}";
  sendJson(200,json);
}

static void handleStatus() {
  const String ping = transactArduino("PING");
  const bool arduinoReady = ping == "OK,PONG";
  StaticJsonDocument<256> doc;
  doc["success"] = true;
  doc["board"] = "ESP32-S3-CAM";
  doc["wifi"] = WiFi.status() == WL_CONNECTED;
  doc["arduino"] = arduinoReady;
  doc["camera"] = cameraReady;
  doc["ip"] = WiFi.localIP().toString();
  doc["rssi"] = WiFi.RSSI();
  String json;
  serializeJson(doc,json);
  sendJson(200,json);
}

static void handleStop() {
  const String reply =transactArduino("STOP");
  sendArduinoResult(reply);
}

void registerRoutes() {
  server.on("/move",HTTP_POST,handleMove);
  server.on("/motor",HTTP_POST,handleMotor);
  server.on("/pump",HTTP_POST,handlePump);
  server.on("/radar",HTTP_POST,handleRadar);
  server.on("/turret",HTTP_POST,handleTurret);
  server.on("/sensors",HTTP_GET,handleSensors);
  server.on("/status",HTTP_GET,handleStatus);
  server.on("/stop",HTTP_POST,handleStop);

  server.on("/move",HTTP_OPTIONS,handleOptions);
  server.on("/motor",HTTP_OPTIONS,handleOptions);
  server.on("/pump",HTTP_OPTIONS,handleOptions);
  server.on("/radar",HTTP_OPTIONS,handleOptions);
  server.on("/turret",HTTP_OPTIONS,handleOptions);
  server.on("/stop",HTTP_OPTIONS,handleOptions);

  // Rută inexistentă
  server.onNotFound(
    []() {
      sendJson(
        404,
        "{\"success\":false,"
        "\"error\":\"Ruta inexistenta\"}");
    });
}

void startControlServer() {
  server.begin();
  Serial.println(
    "[HTTP] Server control pornit pe portul 80");
}
void handleControlServer() {
  server.handleClient();
}