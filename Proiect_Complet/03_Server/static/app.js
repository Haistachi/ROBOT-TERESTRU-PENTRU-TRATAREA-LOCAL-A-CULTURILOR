let sensorTimer = null;
let appSettings = {
    esp32Ip: "192.168.1.100",
    cameraUrl: "http://192.168.1.100:81/stream",
    refreshInterval: 1000
};

function nowTime() {
    return new Date().toLocaleTimeString("ro-RO", { hour12: false });
}

function setStatus(message, type = "info") {
    const status = document.getElementById("systemStatus");
    status.innerText = message;
    status.dataset.type = type;
}

function addLog(message, type = "info") {
    const log = document.getElementById("eventLog");
    if (!log) return;

    const item = document.createElement("li");
    item.className = "log-item " + type;
    item.innerHTML = `<span>${nowTime()}</span><strong>${message}</strong>`;
    log.prepend(item);

    while (log.children.length > 40) {
        log.removeChild(log.lastChild);
    }
}

function clearLog() {
    document.getElementById("eventLog").innerHTML = "";
    addLog("Jurnal curățat", "info");
}

function getSpeedValue(id) {
    return parseInt(document.getElementById(id).value);
}

function postJson(url, data) {
    return fetch(url, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(data)
    }).then(response => response.json());
}

function updateConnectionPill(name, state) {
    const statusElement = document.getElementById(name + "Status");
    const pill = document.getElementById(name + "Pill");
    if (!statusElement || !pill) return;

    const dot = pill.querySelector(".dot");
    const normalized = state === true || state === "online" || state === "ok";

    statusElement.innerText = normalized ? "conectat" : "deconectat";
    dot.className = "dot " + (normalized ? "online" : "offline");
}

function updateLastUpdate() {
    document.getElementById("lastUpdate").innerText = nowTime();
}

function loadSettings() {
    const saved = localStorage.getItem("robotDashboardSettings");
    if (saved) {
        appSettings = { ...appSettings, ...JSON.parse(saved) };
    }

    document.getElementById("esp32IpInput").value = appSettings.esp32Ip;
    document.getElementById("cameraUrlInput").value = appSettings.cameraUrl;
    document.getElementById("refreshIntervalInput").value = appSettings.refreshInterval.toString();
    document.getElementById("cameraStream").src = appSettings.cameraUrl;
}

async function saveSettings() {
    appSettings.esp32Ip = document.getElementById("esp32IpInput").value.trim() || appSettings.esp32Ip;
    appSettings.cameraUrl = document.getElementById("cameraUrlInput").value.trim() || appSettings.cameraUrl;
    appSettings.refreshInterval = parseInt(document.getElementById("refreshIntervalInput").value);

    const response = await postJson("/api/settings", { esp32_ip: appSettings.esp32Ip, esp32_port: 80 });
    if (response.success === false) {
        setStatus(response.error || "Setările nu au putut fi salvate.", "error");
        return;
    }
    localStorage.setItem("robotDashboardSettings", JSON.stringify(appSettings));
    reloadCamera();
    restartSensorTimer();
    addLog("Setările interfeței au fost salvate", "success");
    setStatus("Setările au fost salvate local în browser.", "success");
}

function reloadCamera() {
    const camera = document.getElementById("cameraStream");
    camera.src = appSettings.cameraUrl + (appSettings.cameraUrl.includes("?") ? "&" : "?") + "t=" + Date.now();
    addLog("Stream cameră reîncărcat", "info");
}

function emergencyStop() {
    activeMovementKey = null;
    pumpKeyboardState = false;
    turretKeyboardState.H = "stop";
    turretKeyboardState.V = "stop";

    postJson("/control/emergency-stop", {}).then(data => {
        const ok = data.success !== false;
        setStatus(ok ? "STOP URGENȚĂ executat." : (data.error || "STOP eșuat."), ok ? "success" : "error");
        addLog("STOP URGENȚĂ activat", ok ? "danger" : "error");
    }).catch(() => setStatus("Serverul nu răspunde la STOP.", "error"));
}

function moveRobot(direction) {
    const speed = getSpeedValue("moveSpeed");

    postJson("/control/move", { direction: direction, speed: speed })
        .then(data => {
            const ok = data.success !== false;
            setStatus("Comandă deplasare: " + direction + " | succes: " + ok, ok ? "success" : "error");
            addLog("Deplasare: " + direction + " PWM=" + speed, ok ? "success" : "error");
        })
        .catch(() => {
            setStatus("Eroare la trimiterea comenzii de deplasare.", "error");
            addLog("Eroare comandă deplasare", "error");
        });
}

function sendMotorControl() {
    const leftDirection = document.getElementById("leftDirection").value;
    const rightDirection = document.getElementById("rightDirection").value;
    const leftSpeed = getSpeedValue("leftSpeed");
    const rightSpeed = getSpeedValue("rightSpeed");

    postJson("/control/motor", {
        left_motor: { direction: leftDirection, speed: leftSpeed },
        right_motor: { direction: rightDirection, speed: rightSpeed }
    })
        .then(data => {
            const ok = data.success !== false;
            setStatus("Control individual motoare trimis | succes: " + ok, ok ? "success" : "error");
            addLog(`Motoare: L=${leftDirection}/${leftSpeed}, R=${rightDirection}/${rightSpeed}`, ok ? "success" : "error");
        })
        .catch(() => {
            setStatus("Eroare la controlul individual al motoarelor.", "error");
            addLog("Eroare control motoare", "error");
        });
}

function controlPump(state) {
    postJson("/control/pump", { pump: state })
        .then(data => {
            const ok = data.success !== false;
            setStatus("Pompă: " + state + " | succes: " + ok, ok ? "success" : "error");
            addLog("Pompă " + state.toUpperCase(), ok ? "success" : "error");
        })
        .catch(() => {
            setStatus("Eroare la controlul pompei.", "error");
            addLog("Eroare control pompă", "error");
        });
}

function controlTurret(axis, command) {
    postJson("/control/turret", { axis: axis, command: command })
        .then(data => {
            const ok = data.success !== false;
            const label = command === 90 ? "STOP" : `valoare ${command}`;
            setStatus(`Tun ${axis}: ${label}`, ok ? "success" : "error");
            addLog(`Tun ${axis}: ${label}`, ok ? "success" : "error");
        })
        .catch(() => {
            setStatus("Eroare la controlul tunului.", "error");
            addLog("Eroare control tun", "error");
        });
}

function sendRadarControl() {
    const angle = getSpeedValue("radarAngle");

    postJson("/control/radar", { angle: angle })
        .then(data => {
            const ok = data.success !== false;
            setStatus("Radar rotit la " + angle + " grade | succes: " + ok, ok ? "success" : "error");
            addLog("Radar ultrasonic: " + angle + "°", ok ? "success" : "error");
        })
        .catch(() => {
            setStatus("Eroare la controlul radarului ultrasonic.", "error");
            addLog("Eroare control radar", "error");
        });
}

function safeText(id, value) {
    const el = document.getElementById(id);
    if (el) el.innerText = value ?? "-";
}

function updateSensors() {
    fetch("/sensors/status")
        .then(response => response.json())
        .then(data => {
            safeText("ultrasonicValue", data.ultrasonic_cm);
            safeText("irLeftValue", data.ir_left ? "Obstacol" : "Liber");
            safeText("irRightValue", data.ir_right ? "Obstacol" : "Liber");
            safeText("turretHorizontalPosition", data.turret_horizontal);
            safeText("turretVerticalPosition", data.turret_vertical);

            if (data.imu) {
                safeText("accX", data.imu.acc_x);
                safeText("accY", data.imu.acc_y);
                safeText("accZ", data.imu.acc_z);
                safeText("gyroX", data.imu.gyro_x);
                safeText("gyroY", data.imu.gyro_y);
                safeText("gyroZ", data.imu.gyro_z);
            }

            if (data.gps) {
                safeText("gpsAvailable", data.gps.available ? "Da" : "Nu");
                safeText("gpsLat", data.gps.lat);
                safeText("gpsLng", data.gps.lng);
            }

            if (data.battery) {
                safeText("batteryVoltage", data.battery.voltage);
            }

            updateLastUpdate();
        })
        .catch(() => {
            setStatus("Eroare la citirea senzorilor.", "error");
        });
}

function updateSystemStatus() {
    fetch("/system/status")
        .then(response => response.json())
        .then(data => {
            updateConnectionPill("server", true);
            updateConnectionPill("esp32", data.esp32 === true);
            updateConnectionPill("arduino", data.arduino === true);
            updateConnectionPill("camera", data.camera === true);
            updateLastUpdate();
            addLog("Status sistem actualizat", "info");
        })
        .catch(() => {
            updateConnectionPill("server", false);
            setStatus("Serverul Flask nu răspunde.", "error");
            addLog("Eroare status sistem", "error");
        });
}

function bindSlider(sliderId, valueId) {
    const slider = document.getElementById(sliderId);
    const value = document.getElementById(valueId);
    if (!slider || !value) return;

    slider.addEventListener("input", () => {
        value.innerText = slider.value;
    });
}

function restartSensorTimer() {
    if (sensorTimer) clearInterval(sensorTimer);
    sensorTimer = setInterval(updateSensors, appSettings.refreshInterval);
}

const movementKeys = {
    KeyW: "forward",
    KeyS: "backward",
    KeyA: "left",
    KeyD: "right",
    KeyQ: "rotate_left",
    KeyE: "rotate_right"
};
let activeMovementKey = null;
let pumpKeyboardState = false;

// Starea comenzilor de tastatură pentru tun.
// Aceeași tastă apăsată a doua oară oprește axa respectivă.
const turretKeyboardState = {
    H: "stop",
    V: "stop"
};

function toggleTurretFromKeyboard(axis, direction) {
    const sameDirection = turretKeyboardState[axis] === direction;

    if (sameDirection) {
        turretKeyboardState[axis] = "stop";
        controlTurret(axis, 90);
        return;
    }

    turretKeyboardState[axis] = direction;

    // Pentru controlul din tastatură se folosesc doar vitezele lente.
    // 85 = un sens, 97 = sens opus, 90 = stop.
    const command = (direction === "negative") ? 85 : 97;
    controlTurret(axis, command);
}

function isTypingTarget(target) {
    return target instanceof HTMLInputElement || target instanceof HTMLTextAreaElement || target instanceof HTMLSelectElement;
}

document.addEventListener("keydown", event => {
    if (isTypingTarget(event.target) || event.repeat) return;

    if (movementKeys[event.code]) {
        event.preventDefault();
        activeMovementKey = event.code;
        moveRobot(movementKeys[event.code]);
        return;
    }
    if (event.code === "Space") {
        event.preventDefault();
        activeMovementKey = null;
        moveRobot("stop");
        return;
    }
    if (event.code === "KeyP") {
        pumpKeyboardState = !pumpKeyboardState;
        controlPump(pumpKeyboardState ? "on" : "off");
        return;
    }

    // Control tun cu taste de tip toggle:
    // aceeași tastă apăsată din nou oprește axa.
    if (event.code === "KeyJ") {
        toggleTurretFromKeyboard("H", "negative");
        return;
    }
    if (event.code === "KeyL") {
        toggleTurretFromKeyboard("H", "positive");
        return;
    }
    if (event.code === "KeyI") {
        toggleTurretFromKeyboard("V", "negative");
        return;
    }
    if (event.code === "KeyK") {
        toggleTurretFromKeyboard("V", "positive");
        return;
    }

    if (event.code === "Escape") {
        activeMovementKey = null;
        emergencyStop();
    }
});

document.addEventListener("keyup", event => {
    if (movementKeys[event.code] && activeMovementKey === event.code) {
        event.preventDefault();
        activeMovementKey = null;
        moveRobot("stop");
    }
});

window.addEventListener("blur", () => {
    if (activeMovementKey) {
        activeMovementKey = null;
        moveRobot("stop");
    }
});

bindSlider("moveSpeed", "moveSpeedValue");
bindSlider("leftSpeed", "leftSpeedValue");
bindSlider("rightSpeed", "rightSpeedValue");
bindSlider("radarAngle", "radarAngleValue");

loadSettings();
restartSensorTimer();
updateSensors();
updateSystemStatus();
addLog("Interfață client încărcată", "success");
