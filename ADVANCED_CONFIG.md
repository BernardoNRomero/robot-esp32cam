# 🔧 Guía de Configuración Avanzada y Personalización

Este documento contiene configuraciones avanzadas, variaciones del hardware, y optimizaciones adicionales para el robot ESP32-CAM.

---

## 📐 Esquemas de Conexión Detallados

### Opción 1: Alimentación con Regulador L298N

**Ventaja**: Solo necesitas una fuente de alimentación
**Desventaja**: El regulador del L298N debe soportar suficiente corriente

```
Batería 7-12V
    │
    ├─────→ +12V (L298N) ───→ Alimentación motores
    │
    └─────→ GND (L298N)
                │
                ├─────→ GND común
                │
                └─────→ +5V (salida del regulador L298N)
                        │
                        └─────→ 5V (ESP32-CAM)
                        
                        GND ESP32-CAM ←───→ GND común
```

### Opción 2: Alimentación Dual (Recomendado)

**Ventaja**: Mayor estabilidad, sin interferencias
**Desventaja**: Necesitas dos fuentes

```
Fuente 5V/2A                    Batería 7-12V
    │                               │
    ├────→ 5V (ESP32-CAM)          ├────→ +12V (L298N)
    │                               │
    └────→ GND ←───────────────────┴────→ GND
              (GND COMÚN - MUY IMPORTANTE)
```

### Opción 3: Con Power Bank USB

**Ventaja**: Portátil y fácil de recargar
**Desventaja**: Algunos power banks se apagan con consumo bajo

```
Power Bank USB 5V/2A
    │
    └─→ Cable USB a Micro-USB/USB-C
                │
                └─→ Convertidor USB a GPIO
                        │
                        ├────→ 5V (ESP32-CAM)
                        └────→ GND
                               │
                        GND común con L298N
```

---

## 🎛️ Variaciones de Pines

### Si tienes conflictos de pines

Puedes usar otros GPIO disponibles (evitando los de la cámara):

```cpp
// PINES SEGUROS PARA USAR:
// GPIO 2, 12, 13, 14, 15, 16 (son los mejores)

// EVITAR (usados por cámara o boot):
// GPIO 0, 1, 3, 4, 5, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33, 34, 35, 36, 39

// CONFIGURACIÓN ALTERNATIVA:
#define MOTOR_LEFT_FORWARD   2    // IN1
#define MOTOR_LEFT_BACKWARD  14   // IN2
#define MOTOR_RIGHT_FORWARD  15   // IN3
#define MOTOR_RIGHT_BACKWARD 13   // IN4
```

### LED Flash en otros modelos ESP32-CAM

Algunos clones usan pines diferentes:

```cpp
// AI Thinker (original):
#define LED_FLASH 4

// Algunos clones chinos:
#define LED_FLASH 33

// Si no funciona, probar ambos o verificar con multímetro
```

---

## ⚙️ Configuraciones de Cámara

### Para mejorar rendimiento en WiFi lento

```cpp
// En setupCamera(), cambiar:
config.frame_size = FRAMESIZE_CIF;   // 400x296 (más rápido)
config.jpeg_quality = 15;             // Mayor número = menor calidad
config.fb_count = 1;                  // Un solo buffer
```

### Para máxima calidad (WiFi rápido)

```cpp
config.frame_size = FRAMESIZE_SVGA;  // 800x600
config.jpeg_quality = 8;              // Menor = mejor calidad
config.fb_count = 2;                  // Doble buffer
```

### Resoluciones disponibles

```cpp
FRAMESIZE_QQVGA    // 160x120
FRAMESIZE_QCIF     // 176x144
FRAMESIZE_HQVGA    // 240x176
FRAMESIZE_QVGA     // 320x240
FRAMESIZE_CIF      // 400x296
FRAMESIZE_VGA      // 640x480
FRAMESIZE_SVGA     // 800x600
FRAMESIZE_XGA      // 1024x768
FRAMESIZE_SXGA     // 1280x1024
FRAMESIZE_UXGA     // 1600x1200
```

**Nota**: Resoluciones altas requieren más RAM y pueden causar reinicios si no hay suficiente PSRAM.

---

## 🔊 Agregar Buzzer/Bocina

### Hardware

```
ESP32-CAM GPIO 2 ──→ Transistor NPN (Base)
                      │
Transistor (Colector) ─→ Buzzer (+)
                      │
Transistor (Emisor) ──→ GND

Buzzer (-) ────────────→ GND
```

### Código

```cpp
// Al inicio
#define BUZZER_PIN 2

// En setup()
pinMode(BUZZER_PIN, OUTPUT);

// Función para emitir sonido
void beep(int duration) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duration);
  digitalWrite(BUZZER_PIN, LOW);
}

// Usar en los handlers
void handleForward() {
  beep(50);  // Beep corto al mover
  moveForward();
  server.send(200, "text/plain", "OK");
}
```

---

## 📡 Agregar Sensor Ultrasónico (HC-SR04)

### Hardware

```
ESP32-CAM          HC-SR04
──────────────────────────
GPIO 16     ──→    Trigger
GPIO 0      ──→    Echo
5V          ──→    VCC
GND         ──→    GND
```

### Código

```cpp
// Definiciones
#define TRIG_PIN 16
#define ECHO_PIN 0

long duration;
int distance;

// En setup()
pinMode(TRIG_PIN, OUTPUT);
pinMode(ECHO_PIN, INPUT);

// Función para medir distancia
int getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  duration = pulseIn(ECHO_PIN, HIGH, 30000);  // Timeout 30ms
  distance = duration * 0.034 / 2;
  
  return distance;
}

// Handler para leer distancia
void handleDistance() {
  int dist = getDistance();
  String json = "{\"distance\":" + String(dist) + "}";
  server.send(200, "application/json", json);
}

// En setup()
server.on("/distance", handleDistance);

// En la página HTML, agregar JavaScript:
// setInterval(() => {
//   fetch('/distance')
//     .then(r => r.json())
//     .then(data => {
//       document.getElementById('distance').innerText = data.distance + ' cm';
//     });
// }, 500);
```

---

## 🎮 Control con Gamepad/Joystick Virtual

### HTML adicional para joystick

```html
<div class="joystick-container">
  <canvas id="joystick" width="200" height="200"></canvas>
</div>

<script>
const canvas = document.getElementById('joystick');
const ctx = canvas.getContext('2d');
let centerX = canvas.width / 2;
let centerY = canvas.height / 2;
let isDragging = false;

function drawJoystick(x, y) {
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  
  // Base
  ctx.beginPath();
  ctx.arc(centerX, centerY, 80, 0, 2 * Math.PI);
  ctx.strokeStyle = '#ccc';
  ctx.lineWidth = 3;
  ctx.stroke();
  
  // Stick
  ctx.beginPath();
  ctx.arc(x, y, 30, 0, 2 * Math.PI);
  ctx.fillStyle = '#667eea';
  ctx.fill();
}

function handleJoystick(x, y) {
  let dx = x - centerX;
  let dy = y - centerY;
  let angle = Math.atan2(dy, dx);
  let magnitude = Math.min(Math.sqrt(dx*dx + dy*dy), 80);
  
  if (magnitude < 20) {
    sendCommand('stop');
    return;
  }
  
  // Determinar dirección
  let deg = angle * 180 / Math.PI;
  if (deg >= -45 && deg < 45) sendCommand('derecha');
  else if (deg >= 45 && deg < 135) sendCommand('adelante');
  else if (deg >= -135 && deg < -45) sendCommand('atras');
  else sendCommand('izquierda');
}

canvas.addEventListener('mousedown', () => isDragging = true);
canvas.addEventListener('mouseup', () => {
  isDragging = false;
  drawJoystick(centerX, centerY);
  sendCommand('stop');
});
canvas.addEventListener('mousemove', (e) => {
  if (!isDragging) return;
  let rect = canvas.getBoundingClientRect();
  handleJoystick(e.clientX - rect.left, e.clientY - rect.top);
});

// Touch events para móvil
canvas.addEventListener('touchstart', () => isDragging = true);
canvas.addEventListener('touchend', () => {
  isDragging = false;
  drawJoystick(centerX, centerY);
  sendCommand('stop');
});
canvas.addEventListener('touchmove', (e) => {
  e.preventDefault();
  if (!isDragging) return;
  let rect = canvas.getBoundingClientRect();
  let touch = e.touches[0];
  handleJoystick(touch.clientX - rect.left, touch.clientY - rect.top);
});

drawJoystick(centerX, centerY);
</script>
```

---

## 🔐 Agregar Autenticación Web

### Código Arduino

```cpp
// Credenciales
const char* www_username = "admin";
const char* www_password = "tu_password_seguro";

// Función de verificación
bool checkAuth() {
  if (!server.authenticate(www_username, www_password)) {
    server.requestAuthentication();
    return false;
  }
  return true;
}

// Aplicar a todos los handlers
void handleRoot() {
  if (!checkAuth()) return;
  server.send(200, "text/html", MAIN_page);
}

void handleForward() {
  if (!checkAuth()) return;
  moveForward();
  server.send(200, "text/plain", "OK");
}

// ... aplicar a todos los demás handlers
```

---

## 📶 Modo Access Point (Sin Router)

Si quieres que la ESP32-CAM cree su propia red WiFi:

```cpp
// Reemplazar la sección de WiFi en setup()
const char* ap_ssid = "Robot-ESP32";
const char* ap_password = "12345678";  // Mínimo 8 caracteres

void setup() {
  // ... código anterior ...
  
  // Crear Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);  // Siempre será 192.168.4.1
  
  // ... resto del código ...
}
```

Luego:
1. Conectar tu móvil/PC a la red "Robot-ESP32"
2. Usar contraseña "12345678"
3. Abrir navegador en `http://192.168.4.1`

---

## ⚡ Control PWM de Velocidad Variable

### Hardware (conexiones adicionales)

```
ESP32-CAM GPIO 2  ──→  ENA (L298N)
ESP32-CAM GPIO 16 ──→  ENB (L298N)
```

Quitar los jumpers que vienen en ENA y ENB del L298N.

### Código completo con velocidad

```cpp
// Pines PWM
#define MOTOR_LEFT_ENABLE  2
#define MOTOR_RIGHT_ENABLE 16

// Configuración PWM
#define PWM_FREQ 1000
#define PWM_RESOLUTION 8
#define PWM_CHANNEL_LEFT 0
#define PWM_CHANNEL_RIGHT 1

// Variable de velocidad (0-255)
int currentSpeed = 200;  // Velocidad por defecto

void setupMotorsPWM() {
  // Configurar pines digitales
  pinMode(MOTOR_LEFT_FORWARD, OUTPUT);
  pinMode(MOTOR_LEFT_BACKWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_FORWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_BACKWARD, OUTPUT);
  
  // Configurar PWM
  ledcSetup(PWM_CHANNEL_LEFT, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(PWM_CHANNEL_RIGHT, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(MOTOR_LEFT_ENABLE, PWM_CHANNEL_LEFT);
  ledcAttachPin(MOTOR_RIGHT_ENABLE, PWM_CHANNEL_RIGHT);
  
  // Velocidad inicial
  setMotorSpeed(currentSpeed);
  
  stopMotors();
}

void setMotorSpeed(int speed) {
  if (speed < 0) speed = 0;
  if (speed > 255) speed = 255;
  currentSpeed = speed;
  
  ledcWrite(PWM_CHANNEL_LEFT, speed);
  ledcWrite(PWM_CHANNEL_RIGHT, speed);
  
  Serial.printf("Velocidad establecida: %d/255\n", speed);
}

// Handler para controlar velocidad
void handleSpeed() {
  if (server.hasArg("value")) {
    int speed = server.arg("value").toInt();
    setMotorSpeed(speed);
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Falta parámetro 'value'");
  }
}

// En setup(), reemplazar setupMotors() con:
setupMotorsPWM();

// Y agregar la ruta:
server.on("/speed", handleSpeed);
```

### HTML para control de velocidad

Agregar en la sección de controles:

```html
<div class="section">
  <div class="section-title">⚡ Control de Velocidad</div>
  <div class="speed-control">
    <input type="range" 
           id="speedSlider" 
           min="0" 
           max="255" 
           value="200" 
           oninput="updateSpeed(this.value)">
    <p>Velocidad: <span id="speedValue">200</span>/255</p>
  </div>
</div>

<style>
  .speed-control {
    text-align: center;
  }
  
  #speedSlider {
    width: 80%;
    max-width: 400px;
    height: 10px;
    border-radius: 5px;
    outline: none;
    -webkit-appearance: none;
  }
  
  #speedSlider::-webkit-slider-thumb {
    -webkit-appearance: none;
    width: 25px;
    height: 25px;
    border-radius: 50%;
    background: #667eea;
    cursor: pointer;
  }
</style>

<script>
  function updateSpeed(value) {
    document.getElementById('speedValue').innerText = value;
    fetch(`/speed?value=${value}`);
  }
</script>
```

---

## 🌐 Almacenar HTML en SPIFFS

Para no tener el HTML embebido en el código:

### 1. Preparar SPIFFS

1. Instalar plugin: https://github.com/me-no-dev/arduino-esp32fs-plugin
2. Crear carpeta `data/` en el directorio del sketch
3. Crear archivo `data/index.html` con tu HTML
4. En Arduino IDE: `Herramientas` → `ESP32 Sketch Data Upload`

### 2. Modificar código

```cpp
#include "SPIFFS.h"

void setup() {
  // ... código anterior ...
  
  // Inicializar SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("Error montando SPIFFS");
    return;
  }
  
  // ... resto del setup ...
}

void handleRoot() {
  File file = SPIFFS.open("/index.html", "r");
  if (!file) {
    server.send(404, "text/plain", "Archivo no encontrado");
    return;
  }
  
  server.streamFile(file, "text/html");
  file.close();
}
```

---

## 🔋 Monitoreo de Batería

### Hardware

```
Divisor de voltaje:
Batería (+) ───┬──────→ [Resistor 10kΩ] ───┬──→ GPIO 34 (ESP32)
               │                            │
               └──→ Batería usada           └─→ [Resistor 10kΩ] ──→ GND
```

### Código

```cpp
#define BATTERY_PIN 34
#define BATTERY_MAX_VOLTAGE 12.6  // Voltaje máximo de tu batería
#define BATTERY_MIN_VOLTAGE 9.0   // Voltaje mínimo

float readBatteryVoltage() {
  int rawValue = analogRead(BATTERY_PIN);
  float voltage = (rawValue / 4095.0) * 3.3 * 2;  // *2 por divisor de voltaje
  return voltage;
}

int getBatteryPercentage() {
  float voltage = readBatteryVoltage();
  float percentage = ((voltage - BATTERY_MIN_VOLTAGE) / 
                     (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE)) * 100;
  
  if (percentage > 100) percentage = 100;
  if (percentage < 0) percentage = 0;
  
  return (int)percentage;
}

void handleBattery() {
  float voltage = readBatteryVoltage();
  int percentage = getBatteryPercentage();
  
  String json = "{";
  json += "\"voltage\":" + String(voltage, 2) + ",";
  json += "\"percentage\":" + String(percentage);
  json += "}";
  
  server.send(200, "application/json", json);
}

// En setup()
server.on("/battery", handleBattery);
```

### JavaScript en HTML

```html
<div class="battery-indicator">
  <p>Batería: <span id="batteryPercent">--</span>% 
     (<span id="batteryVoltage">--</span>V)</p>
</div>

<script>
  function updateBattery() {
    fetch('/battery')
      .then(r => r.json())
      .then(data => {
        document.getElementById('batteryPercent').innerText = data.percentage;
        document.getElementById('batteryVoltage').innerText = data.voltage.toFixed(2);
      });
  }
  
  // Actualizar cada 10 segundos
  setInterval(updateBattery, 10000);
  updateBattery();  // Primera llamada inmediata
</script>
```

---

## 🎨 Temas de Color Personalizables

### CSS con variables

```html
<style>
  :root {
    --primary-color: #667eea;
    --secondary-color: #764ba2;
    --accent-color: #f093fb;
  }
  
  body {
    background: linear-gradient(135deg, 
                var(--primary-color) 0%, 
                var(--secondary-color) 100%);
  }
  
  .btn-forward {
    background: linear-gradient(135deg, 
                var(--primary-color) 0%, 
                var(--secondary-color) 100%);
  }
</style>
```

### Selector de temas

```html
<select onchange="changeTheme(this.value)">
  <option value="blue">Azul</option>
  <option value="green">Verde</option>
  <option value="red">Rojo</option>
</select>

<script>
  const themes = {
    blue: {
      primary: '#667eea',
      secondary: '#764ba2',
      accent: '#f093fb'
    },
    green: {
      primary: '#11998e',
      secondary: '#38ef7d',
      accent: '#40E0D0'
    },
    red: {
      primary: '#f12711',
      secondary: '#f5af19',
      accent: '#ff6b6b'
    }
  };
  
  function changeTheme(themeName) {
    const theme = themes[themeName];
    document.documentElement.style.setProperty('--primary-color', theme.primary);
    document.documentElement.style.setProperty('--secondary-color', theme.secondary);
    document.documentElement.style.setProperty('--accent-color', theme.accent);
  }
</script>
```

---

## 🗺️ Agregar Control con Teclado

### JavaScript para teclado

```javascript
// Agregar al final del script
let keysPressed = {};

document.addEventListener('keydown', (e) => {
  if (keysPressed[e.key]) return;  // Ya presionado
  
  keysPressed[e.key] = true;
  
  switch(e.key) {
    case 'ArrowUp':
    case 'w':
    case 'W':
      sendCommand('adelante');
      break;
    case 'ArrowDown':
    case 's':
    case 'S':
      sendCommand('atras');
      break;
    case 'ArrowLeft':
    case 'a':
    case 'A':
      sendCommand('izquierda');
      break;
    case 'ArrowRight':
    case 'd':
    case 'D':
      sendCommand('derecha');
      break;
    case ' ':
      sendCommand('stop');
      break;
    case 'l':
    case 'L':
      sendCommand('led_on');
      break;
    case 'k':
    case 'K':
      sendCommand('led_off');
      break;
  }
  
  e.preventDefault();
});

document.addEventListener('keyup', (e) => {
  delete keysPressed[e.key];
  
  if (['ArrowUp', 'ArrowDown', 'ArrowLeft', 'ArrowRight', 'w', 's', 'a', 'd'].includes(e.key)) {
    sendCommand('stop');
  }
});
```

---

## 📊 Telemetría y Logs

### Código para registro de eventos

```cpp
#define MAX_LOGS 50
String logs[MAX_LOGS];
int logIndex = 0;

void addLog(String message) {
  String timestamp = String(millis() / 1000) + "s";
  logs[logIndex] = timestamp + ": " + message;
  logIndex = (logIndex + 1) % MAX_LOGS;
  Serial.println(message);
}

void handleLogs() {
  String html = "<html><body><h1>Logs del Robot</h1><pre>";
  
  for (int i = 0; i < MAX_LOGS; i++) {
    int idx = (logIndex + i) % MAX_LOGS;
    if (logs[idx].length() > 0) {
      html += logs[idx] + "\n";
    }
  }
  
  html += "</pre><a href='/'>Volver</a></body></html>";
  server.send(200, "text/html", html);
}

// Usar en lugar de Serial.println
void handleForward() {
  addLog("Comando: Adelante");
  moveForward();
  server.send(200, "text/plain", "OK");
}

// En setup()
server.on("/logs", handleLogs);
```

---

## 🧪 Testing y Diagnóstico

### Página de diagnóstico

```cpp
void handleDiagnostics() {
  String html = "<html><head><meta charset='UTF-8'></head><body>";
  html += "<h1>Diagnósticos del Sistema</h1>";
  html += "<table border='1' cellpadding='5'>";
  
  html += "<tr><td>IP Address</td><td>" + WiFi.localIP().toString() + "</td></tr>";
  html += "<tr><td>WiFi RSSI</td><td>" + String(WiFi.RSSI()) + " dBm</td></tr>";
  html += "<tr><td>Uptime</td><td>" + String(millis() / 1000) + " segundos</td></tr>";
  html += "<tr><td>Free Heap</td><td>" + String(ESP.getFreeHeap()) + " bytes</td></tr>";
  html += "<tr><td>PSRAM</td><td>" + String(psramFound() ? "Sí" : "No") + "</td></tr>";
  
  if (psramFound()) {
    html += "<tr><td>Free PSRAM</td><td>" + String(ESP.getFreePsram()) + " bytes</td></tr>";
  }
  
  html += "<tr><td>LED State</td><td>" + String(ledState ? "ON" : "OFF") + "</td></tr>";
  html += "<tr><td>Chip Model</td><td>" + String(ESP.getChipModel()) + "</td></tr>";
  html += "<tr><td>CPU Freq</td><td>" + String(ESP.getCpuFreqMHz()) + " MHz</td></tr>";
  
  html += "</table>";
  html += "<br><a href='/'>Volver al inicio</a>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

// En setup()
server.on("/diagnostics", handleDiagnostics);
```

---

## 💡 Tips de Optimización

### 1. Reducir latencia

```cpp
// En handleStream(), reducir delay
delay(5);  // En lugar de 10

// O eliminar completamente para máxima velocidad (puede causar sobrecarga)
```

### 2. Aumentar FPS

```cpp
// En setupCamera()
config.xclk_freq_hz = 20000000;  // Probar con 20MHz
config.jpeg_quality = 12;         // Balance calidad/velocidad
```

### 3. Reducir consumo de energía

```cpp
// Bajar frecuencia de CPU si no necesitas máximo rendimiento
setCpuFrequencyMhz(160);  // En lugar de 240MHz por defecto

// En setup(), después de inicializar
Serial.println("Frecuencia CPU: " + String(getCpuFrequencyMhz()) + " MHz");
```

### 4. Watchdog Timer para mayor estabilidad

```cpp
#include "esp_task_wdt.h"

void setup() {
  // ... código anterior ...
  
  // Configurar Watchdog (10 segundos)
  esp_task_wdt_init(10, true);
  esp_task_wdt_add(NULL);
}

void loop() {
  // Reset watchdog
  esp_task_wdt_reset();
  
  server.handleClient();
  delay(1);
}
```

---

## 🎯 Proyecto Completo de Ejemplo: Robot Autónomo

Combinando sensores ultrasónicos y lógica de evitación de obstáculos:

```cpp
// Variables globales
bool autonomousMode = false;
unsigned long lastSensorCheck = 0;

void autonomousDrive() {
  if (!autonomousMode) return;
  
  unsigned long now = millis();
  if (now - lastSensorCheck < 200) return;  // Checkear cada 200ms
  lastSensorCheck = now;
  
  int distance = getDistance();
  
  if (distance < 20) {  // Obstáculo cercano
    stopMotors();
    delay(200);
    
    // Retroceder
    moveBackward();
    delay(500);
    
    // Girar aleatoriamente
    if (random(2) == 0) {
      turnLeft();
    } else {
      turnRight();
    }
    delay(300);
    
  } else {
    // Seguir adelante
    moveForward();
  }
}

void handleAutonomous() {
  if (server.hasArg("enable")) {
    autonomousMode = (server.arg("enable") == "1");
    server.send(200, "text/plain", autonomousMode ? "Modo autónomo activado" : "Modo autónomo desactivado");
  }
}

void loop() {
  server.handleClient();
  
  if (autonomousMode) {
    autonomousDrive();
  }
  
  delay(1);
}

// En setup()
server.on("/autonomous", handleAutonomous);
```

---

**Este archivo contiene configuraciones avanzadas. Experimenta con cuidado y prueba cada cambio individualmente.**

---

**Última actualización**: Mayo 2026
