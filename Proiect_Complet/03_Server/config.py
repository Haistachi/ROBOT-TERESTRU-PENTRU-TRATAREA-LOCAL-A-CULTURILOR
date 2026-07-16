import os

ESP32_IP = os.getenv("ESP32_IP", "192.168.1.100")
ESP32_PORT = int(os.getenv("ESP32_PORT", "80"))
HTTP_TIMEOUT = float(os.getenv("ROBOT_HTTP_TIMEOUT", "2.0"))
MOCK_MODE = os.getenv("ROBOT_MOCK_MODE", "0") == "1"
