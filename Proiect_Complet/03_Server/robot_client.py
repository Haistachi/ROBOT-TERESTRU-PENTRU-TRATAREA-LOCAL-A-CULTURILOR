from __future__ import annotations

import threading
from dataclasses import dataclass
from typing import Any

import requests

from config import ESP32_IP, ESP32_PORT, HTTP_TIMEOUT, MOCK_MODE


@dataclass
class RobotConfig:
    ip: str = ESP32_IP
    port: int = ESP32_PORT

    @property
    def base_url(self) -> str:
        return f"http://{self.ip}:{self.port}"


class RobotClient:
    def __init__(self) -> None:
        self._config = RobotConfig()
        self._lock = threading.Lock()
        self.mock_mode = MOCK_MODE

    def get_config(self) -> dict[str, Any]:
        with self._lock:
            return {
                "esp32_ip": self._config.ip,
                "esp32_port": self._config.port,
                "camera_url": f"http://{self._config.ip}:81/stream",
                "mock_mode": self.mock_mode,
            }

    def update_config(self, ip: str, port: int = 80) -> dict[str, Any]:
        ip = ip.strip()
        if not ip or any(ch not in "0123456789." for ch in ip):
            raise ValueError("Adresa IP nu este validă")
        if not 1 <= port <= 65535:
            raise ValueError("Portul nu este valid")
        with self._lock:
            self._config = RobotConfig(ip=ip, port=port)
        return self.get_config()

    def request(self, endpoint: str, payload: dict[str, Any] | None = None) -> dict[str, Any]:
        if self.mock_mode:
            return self._mock(endpoint, payload)
        with self._lock:
            url = f"{self._config.base_url}{endpoint}"
        try:
            response = requests.get(url, timeout=HTTP_TIMEOUT) if payload is None else requests.post(url, json=payload, timeout=HTTP_TIMEOUT)
            try:
                data = response.json()
            except ValueError:
                data = {"success": False, "error": "ESP32 a trimis un răspuns non-JSON"}
            data.setdefault("success", response.ok)
            if not response.ok:
                data.setdefault("error", f"ESP32 HTTP {response.status_code}")
            return data
        except requests.RequestException as exc:
            return {"success": False, "error": "ESP32 nu răspunde", "details": str(exc)}

    @staticmethod
    def _mock(endpoint: str, payload: dict[str, Any] | None) -> dict[str, Any]:
        if endpoint == "/sensors":
            return {"success": True, "ultrasonic_cm": 42.5, "ir_left": False, "ir_right": False, "pump": False, "radar_angle": 90, "turret_horizontal": 90, "turret_vertical": 90}
        if endpoint == "/status":
            return {"success": True, "wifi": True, "arduino": True, "camera": True, "ip": "mock", "rssi": -45}
        return {"success": True, "mock": True, "endpoint": endpoint, "payload": payload or {}}
