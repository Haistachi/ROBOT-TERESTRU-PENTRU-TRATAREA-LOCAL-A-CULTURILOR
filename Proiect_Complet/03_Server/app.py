from __future__ import annotations

from flask import Flask, jsonify, render_template, request

from robot_client import RobotClient

app = Flask(__name__)
robot = RobotClient()


def json_body() -> dict:
    return request.get_json(silent=True) or {}


@app.get("/")
def index():
    return render_template("index.html")


@app.get("/api/settings")
def get_settings():
    return jsonify({"success": True, **robot.get_config()})


@app.post("/api/settings")
def set_settings():
    data = json_body()
    try:
        config = robot.update_config(str(data.get("esp32_ip", "")), int(data.get("esp32_port", 80)))
        return jsonify({"success": True, **config})
    except (ValueError, TypeError) as exc:
        return jsonify({"success": False, "error": str(exc)}), 400


@app.post("/control/move")
def control_move():
    data = json_body()
    direction = str(data.get("direction", "stop")).lower()
    allowed = {"forward", "backward", "left", "right", "rotate_left", "rotate_right", "stop"}
    if direction not in allowed:
        return jsonify({"success": False, "error": "Direcție invalidă"}), 400
    speed = max(0, min(int(data.get("speed", 150)), 255))
    return jsonify(robot.request("/move", {"direction": direction, "speed": speed}))


@app.post("/control/motor")
def control_motor():
    data = json_body()
    return jsonify(robot.request("/motor", {"left_motor": data.get("left_motor", {}), "right_motor": data.get("right_motor", {})}))


@app.post("/control/pump")
def control_pump():
    state = str(json_body().get("pump", "off")).lower()
    if state not in {"on", "off"}:
        return jsonify({"success": False, "error": "Stare pompă invalidă"}), 400
    return jsonify(robot.request("/pump", {"pump": state}))


@app.post("/control/turret")
def control_turret():
    data = json_body()
    axis = str(data.get("axis", "")).upper()
    command = int(data.get("command", 90))
    if axis not in {"H", "V"}:
        return jsonify({"success": False, "error": "Axa turelei este invalidă"}), 400
    if command not in {82, 85, 90, 97, 100}:
        return jsonify({"success": False, "error": "Comanda servoului este invalidă"}), 400
    return jsonify(robot.request("/turret", {"axis": axis, "command": command}))


@app.post("/control/radar")
def control_radar():
    angle = max(0, min(int(json_body().get("angle", 90)), 180))
    return jsonify(robot.request("/radar", {"angle": angle}))


@app.post("/control/emergency-stop")
def emergency_stop():
    return jsonify(robot.request("/stop", {}))


@app.get("/sensors/status")
def sensors_status():
    return jsonify(robot.request("/sensors"))


@app.get("/system/status")
def system_status():
    details = robot.request("/status")
    return jsonify({"server": True, "esp32": details.get("success", False), "arduino": bool(details.get("arduino", False)), "camera": bool(details.get("camera", False)), "details": details})


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=False, threaded=True)
