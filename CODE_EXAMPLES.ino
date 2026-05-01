/*
 * EJEMPLOS DE CÓDIGO ADICIONALES
 * 
 * Este archivo contiene funciones y mejoras opcionales que puedes
 * agregar a tu robot ESP32-CAM. Cada sección es independiente.
 * 
 * Instrucciones:
 * 1. Copia la sección que te interese
 * 2. Pégala en el archivo principal (.ino)
 * 3. Sigue las instrucciones específicas de cada función
 */

// ============================================================================
// EJEMPLO 1: CONTROL DE VELOCIDAD PWM
// ============================================================================
// Requisito: Conectar GPIO 2 a ENA y GPIO 16 a ENB del L298N
// Quitar los jumpers de ENA y ENB del L298N

#define MOTOR_LEFT_ENABLE  2
#define MOTOR_RIGHT_ENABLE 16
#define PWM_FREQ 1000
#define PWM_RESOLUTION 8
#define PWM_CHANNEL_LEFT 0
#define PWM_CHANNEL_RIGHT 1

int motorSpeed = 200;  // 0-255

void setupPWM() {
  ledcSetup(PWM_CHANNEL_LEFT, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(PWM_CHANNEL_RIGHT, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(MOTOR_LEFT_ENABLE, PWM_CHANNEL_LEFT);
  ledcAttachPin(MOTOR_RIGHT_ENABLE, PWM_CHANNEL_RIGHT);
  ledcWrite(PWM_CHANNEL_LEFT, motorSpeed);
  ledcWrite(PWM_CHANNEL_RIGHT, motorSpeed);
}

void setSpeed(int speed) {
  motorSpeed = constrain(speed, 0, 255);
  ledcWrite(PWM_CHANNEL_LEFT, motorSpeed);
  ledcWrite(PWM_CHANNEL_RIGHT, motorSpeed);
}

void handleSpeed() {
  if (server.hasArg("value")) {
    setSpeed(server.arg("value").toInt());
    server.send(200, "text/plain", "OK");
  }
}

// En setup(): 
// setupPWM();
// server.on("/speed", handleSpeed);

// HTML para slider de velocidad:
/*
<input type="range" min="0" max="255" value="200" 
       oninput="fetch(`/speed?value=${this.value}`)">
*/

// ============================================================================
// EJEMPLO 2: SENSOR ULTRASÓNICO HC-SR04
// ============================================================================
// Conexiones: GPIO 16 → TRIG, GPIO 0 → ECHO

#define TRIG_PIN 16
#define ECHO_PIN 0

void setupUltrasonic() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

int getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  int distance = duration * 0.034 / 2;
  
  return (distance > 0 && distance < 400) ? distance : -1;
}

void handleDistance() {
  int dist = getDistance();
  String json = "{\"distance\":" + String(dist) + "}";
  server.send(200, "application/json", json);
}

// En setup():
// setupUltrasonic();
// server.on("/distance", handleDistance);

// JavaScript para actualizar en HTML:
/*
setInterval(() => {
  fetch('/distance')
    .then(r => r.json())
    .then(d => console.log('Distancia:', d.distance, 'cm'));
}, 500);
*/

// ============================================================================
// EJEMPLO 3: MODO AUTÓNOMO CON EVITACIÓN DE OBSTÁCULOS
// ============================================================================
// Requiere: Sensor ultrasónico (Ejemplo 2)

bool autonomousMode = false;
unsigned long lastCheck = 0;

void autonomousDrive() {
  if (!autonomousMode) return;
  
  if (millis() - lastCheck < 200) return;
  lastCheck = millis();
  
  int dist = getDistance();
  
  if (dist < 20 && dist > 0) {
    stopMotors();
    delay(200);
    moveBackward();
    delay(500);
    
    if (random(2)) turnLeft();
    else turnRight();
    delay(400);
  } else {
    moveForward();
  }
}

void handleAutonomous() {
  if (server.hasArg("enable")) {
    autonomousMode = (server.arg("enable") == "1");
    server.send(200, "text/plain", autonomousMode ? "ON" : "OFF");
  }
}

// En loop(), después de server.handleClient():
// autonomousDrive();

// En setup():
// server.on("/autonomous", handleAutonomous);

// Botón HTML:
/*
<button onclick="fetch('/autonomous?enable=1')">Modo Auto ON</button>
<button onclick="fetch('/autonomous?enable=0')">Modo Auto OFF</button>
*/

// ============================================================================
// EJEMPLO 4: SERVOMOTOR PARA TORRETA DE CÁMARA
// ============================================================================
// Requiere: #include <ESP32Servo.h>
// Conexiones: GPIO 2 → Señal servo PAN, GPIO 16 → Señal servo TILT

#include <ESP32Servo.h>

Servo servoPan;
Servo servoTilt;
int panAngle = 90;
int tiltAngle = 90;

void setupServos() {
  servoPan.attach(2);
  servoTilt.attach(16);
  servoPan.write(panAngle);
  servoTilt.write(tiltAngle);
}

void handleServo() {
  if (server.hasArg("pan")) {
    panAngle = constrain(server.arg("pan").toInt(), 0, 180);
    servoPan.write(panAngle);
  }
  if (server.hasArg("tilt")) {
    tiltAngle = constrain(server.arg("tilt").toInt(), 0, 180);
    servoTilt.write(tiltAngle);
  }
  server.send(200, "text/plain", "OK");
}

// En setup():
// setupServos();
// server.on("/servo", handleServo);

// HTML con sliders:
/*
<label>PAN: <input type="range" min="0" max="180" value="90" 
             oninput="fetch(`/servo?pan=${this.value}`)"></label><br>
<label>TILT: <input type="range" min="0" max="180" value="90" 
              oninput="fetch(`/servo?tilt=${this.value}`)"></label>
*/

// ============================================================================
// EJEMPLO 5: CAPTURA DE FOTOS
// ============================================================================

void handleCapture() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    server.send(500, "text/plain", "Error capturando imagen");
    return;
  }
  
  server.sendHeader("Content-Disposition", "attachment; filename=foto.jpg");
  server.send_P(200, "image/jpeg", (const char *)fb->buf, fb->len);
  esp_camera_fb_return(fb);
  
  Serial.println("Foto capturada");
}

// En setup():
// server.on("/capture", handleCapture);

// Botón HTML:
/*
<button onclick="window.open('/capture', '_blank')">📷 Tomar Foto</button>
*/

// ============================================================================
// EJEMPLO 6: MONITOREO DE BATERÍA
// ============================================================================
// Conexión: Divisor de voltaje desde batería a GPIO 34
// Resistencias: 10kΩ y 10kΩ en serie

#define BATTERY_PIN 34
#define BATTERY_MAX 12.6  // Ajustar según tu batería
#define BATTERY_MIN 9.0

float getBatteryVoltage() {
  int raw = analogRead(BATTERY_PIN);
  float voltage = (raw / 4095.0) * 3.3 * 2;  // *2 por divisor
  return voltage;
}

int getBatteryPercent() {
  float v = getBatteryVoltage();
  float percent = ((v - BATTERY_MIN) / (BATTERY_MAX - BATTERY_MIN)) * 100;
  return constrain(percent, 0, 100);
}

void handleBattery() {
  String json = "{";
  json += "\"voltage\":" + String(getBatteryVoltage(), 2) + ",";
  json += "\"percent\":" + String(getBatteryPercent());
  json += "}";
  server.send(200, "application/json", json);
}

// En setup():
// server.on("/battery", handleBattery);

// JavaScript:
/*
setInterval(() => {
  fetch('/battery')
    .then(r => r.json())
    .then(d => {
      console.log(`Batería: ${d.percent}% (${d.voltage}V)`);
    });
}, 10000);
*/

// ============================================================================
// EJEMPLO 7: BUZZER/BOCINA
// ============================================================================
// Conexión: GPIO 2 → Base de transistor NPN → Buzzer

#define BUZZER_PIN 2

void setupBuzzer() {
  pinMode(BUZZER_PIN, OUTPUT);
}

void beep(int duration) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duration);
  digitalWrite(BUZZER_PIN, LOW);
}

void beepPattern(int times, int duration, int pause) {
  for (int i = 0; i < times; i++) {
    beep(duration);
    delay(pause);
  }
}

void handleBeep() {
  beep(100);
  server.send(200, "text/plain", "OK");
}

// Usar en los handlers de movimiento:
/*
void handleForward() {
  beep(50);
  moveForward();
  server.send(200, "text/plain", "OK");
}
*/

// En setup():
// setupBuzzer();
// server.on("/beep", handleBeep);

// ============================================================================
// EJEMPLO 8: LOGS Y TELEMETRÍA
// ============================================================================

#define MAX_LOGS 50
String logs[MAX_LOGS];
int logIndex = 0;

void addLog(String message) {
  String timestamp = String(millis() / 1000) + "s";
  logs[logIndex] = timestamp + " - " + message;
  logIndex = (logIndex + 1) % MAX_LOGS;
  Serial.println(message);
}

void handleLogs() {
  String html = "<html><head><meta charset='UTF-8'>";
  html += "<meta http-equiv='refresh' content='5'>";  // Auto-refresh
  html += "</head><body>";
  html += "<h1>📊 Logs del Robot</h1>";
  html += "<pre style='font-family: monospace; background: #f5f5f5; padding: 10px;'>";
  
  for (int i = 0; i < MAX_LOGS; i++) {
    int idx = (logIndex + i) % MAX_LOGS;
    if (logs[idx].length() > 0) {
      html += logs[idx] + "\n";
    }
  }
  
  html += "</pre>";
  html += "<br><a href='/'>← Volver</a>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

// Reemplazar Serial.println() con addLog():
/*
void handleForward() {
  addLog("Moviendo adelante");
  moveForward();
  server.send(200, "text/plain", "OK");
}
*/

// En setup():
// server.on("/logs", handleLogs);

// ============================================================================
// EJEMPLO 9: PÁGINA DE DIAGNÓSTICO
// ============================================================================

void handleDiagnostics() {
  String html = "<html><head><meta charset='UTF-8'></head><body>";
  html += "<h1>🔧 Diagnósticos del Sistema</h1>";
  html += "<table border='1' cellpadding='10' style='border-collapse: collapse;'>";
  html += "<tr><th>Parámetro</th><th>Valor</th></tr>";
  
  html += "<tr><td>IP Address</td><td>" + WiFi.localIP().toString() + "</td></tr>";
  html += "<tr><td>MAC Address</td><td>" + WiFi.macAddress() + "</td></tr>";
  html += "<tr><td>SSID</td><td>" + WiFi.SSID() + "</td></tr>";
  html += "<tr><td>WiFi RSSI</td><td>" + String(WiFi.RSSI()) + " dBm</td></tr>";
  html += "<tr><td>Uptime</td><td>" + String(millis() / 1000) + " segundos</td></tr>";
  html += "<tr><td>Free Heap</td><td>" + String(ESP.getFreeHeap() / 1024) + " KB</td></tr>";
  html += "<tr><td>PSRAM</td><td>" + String(psramFound() ? "Sí" : "No") + "</td></tr>";
  
  if (psramFound()) {
    html += "<tr><td>Free PSRAM</td><td>" + String(ESP.getFreePsram() / 1024) + " KB</td></tr>";
  }
  
  html += "<tr><td>Chip Model</td><td>" + String(ESP.getChipModel()) + "</td></tr>";
  html += "<tr><td>CPU Frequency</td><td>" + String(ESP.getCpuFreqMHz()) + " MHz</td></tr>";
  html += "<tr><td>Flash Size</td><td>" + String(ESP.getFlashChipSize() / 1024 / 1024) + " MB</td></tr>";
  html += "<tr><td>Sketch Size</td><td>" + String(ESP.getSketchSize() / 1024) + " KB</td></tr>";
  html += "<tr><td>Free Sketch Space</td><td>" + String(ESP.getFreeSketchSpace() / 1024) + " KB</td></tr>";
  
  html += "</table>";
  html += "<br><a href='/'>← Volver</a>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

// En setup():
// server.on("/diagnostics", handleDiagnostics);

// ============================================================================
// EJEMPLO 10: IP ESTÁTICA
// ============================================================================
// Útil para siempre usar la misma IP

void setupStaticIP() {
  IPAddress local_IP(192, 168, 1, 100);  // Tu IP deseada
  IPAddress gateway(192, 168, 1, 1);     // IP de tu router
  IPAddress subnet(255, 255, 255, 0);
  IPAddress primaryDNS(8, 8, 8, 8);      // Google DNS (opcional)
  
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS)) {
    Serial.println("Error configurando IP estática");
  }
}

// Llamar ANTES de WiFi.begin() en setup():
// setupStaticIP();
// WiFi.begin(ssid, password);

// ============================================================================
// EJEMPLO 11: MODO ACCESS POINT (SIN ROUTER)
// ============================================================================
// La ESP32 crea su propia red WiFi

void setupAccessPoint() {
  const char* ap_ssid = "Robot-ESP32-CAM";
  const char* ap_password = "12345678";  // Mínimo 8 caracteres
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);  // Siempre 192.168.4.1
}

// REEMPLAZAR el WiFi.begin() en setup() con:
// setupAccessPoint();

// Luego conectarte desde tu móvil a la red "Robot-ESP32-CAM"
// y abrir http://192.168.4.1

// ============================================================================
// EJEMPLO 12: AUTENTICACIÓN WEB BÁSICA
// ============================================================================

const char* www_username = "admin";
const char* www_password = "robot123";

bool checkAuth() {
  if (!server.authenticate(www_username, www_password)) {
    server.requestAuthentication(DIGEST_AUTH);
    return false;
  }
  return true;
}

// Agregar al inicio de TODOS los handlers:
/*
void handleRoot() {
  if (!checkAuth()) return;
  server.send(200, "text/html", MAIN_page);
}

void handleForward() {
  if (!checkAuth()) return;
  moveForward();
  server.send(200, "text/plain", "OK");
}
*/

// ============================================================================
// EJEMPLO 13: WATCHDOG TIMER (ANTI-CUELGUE)
// ============================================================================
// Reinicia automáticamente si el programa se cuelga

#include "esp_task_wdt.h"

void setupWatchdog() {
  esp_task_wdt_init(10, true);  // 10 segundos timeout
  esp_task_wdt_add(NULL);
  Serial.println("Watchdog activado");
}

// En setup():
// setupWatchdog();

// En loop(), al final:
// esp_task_wdt_reset();  // Reset del watchdog

// ============================================================================
// EJEMPLO 14: CONTROL CON TECLADO (JavaScript)
// ============================================================================
// Agregar al final del <script> en HTML:

/*
let keysPressed = {};

document.addEventListener('keydown', (e) => {
  if (keysPressed[e.key]) return;
  keysPressed[e.key] = true;
  
  switch(e.key.toLowerCase()) {
    case 'arrowup':
    case 'w':
      sendCommand('adelante');
      break;
    case 'arrowdown':
    case 's':
      sendCommand('atras');
      break;
    case 'arrowleft':
    case 'a':
      sendCommand('izquierda');
      break;
    case 'arrowright':
    case 'd':
      sendCommand('derecha');
      break;
    case ' ':
      sendCommand('stop');
      break;
    case 'l':
      sendCommand('led_on');
      break;
    case 'k':
      sendCommand('led_off');
      break;
  }
  e.preventDefault();
});

document.addEventListener('keyup', (e) => {
  delete keysPressed[e.key];
  if (['arrowup', 'arrowdown', 'arrowleft', 'arrowright', 'w', 's', 'a', 'd'].includes(e.key.toLowerCase())) {
    sendCommand('stop');
  }
});
*/

// ============================================================================
// EJEMPLO 15: REDUCIR CONSUMO DE ENERGÍA
// ============================================================================

void reducePower() {
  // Reducir frecuencia de CPU
  setCpuFrequencyMhz(160);  // En lugar de 240MHz
  
  // Reducir potencia WiFi
  WiFi.setTxPower(WIFI_POWER_11dBm);  // En lugar de 19.5dBm
  
  Serial.println("Modo bajo consumo activado");
}

// Llamar en setup() si necesitas ahorrar batería:
// reducePower();

// ============================================================================
// EJEMPLO 16: AJUSTES FINOS DE CÁMARA
// ============================================================================
// Agregar después de esp_camera_init() en setupCamera():

void adjustCameraSettings() {
  sensor_t * s = esp_camera_sensor_get();
  
  // Brillo (-2 a 2)
  s->set_brightness(s, 0);
  
  // Contraste (-2 a 2)
  s->set_contrast(s, 0);
  
  // Saturación (-2 a 2)
  s->set_saturation(s, 0);
  
  // Balance de blancos (0 = off, 1 = on)
  s->set_whitebal(s, 1);
  
  // Control de exposición (0 = off, 1 = on)
  s->set_exposure_ctrl(s, 1);
  
  // Ganancia (0 = off, 1 = on)
  s->set_gain_ctrl(s, 1);
  
  // Espejo horizontal (0 = off, 1 = on)
  s->set_hmirror(s, 0);
  
  // Voltear vertical (0 = off, 1 = on)
  s->set_vflip(s, 0);
  
  // AEC (Auto Exposure Control) DSP (0 = off, 1 = on)
  s->set_aec2(s, 0);
  
  Serial.println("Configuración de cámara ajustada");
}

// Llamar después de esp_camera_init():
// adjustCameraSettings();

// ============================================================================
// EJEMPLO 17: MEDICIÓN DE FRAMERATE
// ============================================================================

unsigned long frameCount = 0;
unsigned long lastFPSCheck = 0;

void measureFPS() {
  frameCount++;
  
  if (millis() - lastFPSCheck >= 1000) {
    Serial.printf("FPS: %lu\n", frameCount);
    frameCount = 0;
    lastFPSCheck = millis();
  }
}

// Llamar en handleStream() dentro del while:
// measureFPS();

// ============================================================================
// EJEMPLO 18: PÁGINA 404 PERSONALIZADA
// ============================================================================

void handleNotFound() {
  String html = "<html><head><meta charset='UTF-8'>";
  html += "<style>body{font-family:Arial;text-align:center;padding:50px;}";
  html += "h1{font-size:100px;margin:0;}p{font-size:24px;}</style>";
  html += "</head><body>";
  html += "<h1>404</h1>";
  html += "<p>❌ Página no encontrada</p>";
  html += "<p>Ruta solicitada: " + server.uri() + "</p>";
  html += "<br><a href='/'>← Volver al inicio</a>";
  html += "</body></html>";
  
  server.send(404, "text/html", html);
}

// En setup():
// server.onNotFound(handleNotFound);

// ============================================================================
// EJEMPLO 19: TIMEOUT DE SEGURIDAD
// ============================================================================
// Detiene los motores automáticamente si no se recibe comando

unsigned long lastCommandTime = 0;
#define COMMAND_TIMEOUT 1000  // 1 segundo

void checkTimeout() {
  if (millis() - lastCommandTime > COMMAND_TIMEOUT) {
    stopMotors();
  }
}

// Actualizar en cada handler de movimiento:
/*
void handleForward() {
  lastCommandTime = millis();
  moveForward();
  server.send(200, "text/plain", "OK");
}
*/

// Llamar en loop():
// checkTimeout();

// ============================================================================
// EJEMPLO 20: INFORMACIÓN DEL SISTEMA EN JSON
// ============================================================================

void handleStatus() {
  String json = "{";
  json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
  json += "\"rssi\":" + String(WiFi.RSSI()) + ",";
  json += "\"uptime\":" + String(millis() / 1000) + ",";
  json += "\"freeHeap\":" + String(ESP.getFreeHeap()) + ",";
  json += "\"cpuFreq\":" + String(ESP.getCpuFreqMHz()) + ",";
  json += "\"ledState\":" + String(ledState ? "true" : "false");
  json += "}";
  
  server.send(200, "application/json", json);
}

// En setup():
// server.on("/status", handleStatus);

// ============================================================================
// NOTAS FINALES
// ============================================================================
/*
 * Para usar estos ejemplos:
 * 
 * 1. Lee el comentario de "Requisito" o "Conexión" de cada función
 * 2. Copia el código que necesites
 * 3. Pégalo en las secciones apropiadas del archivo principal
 * 4. Sigue las instrucciones específicas (// En setup:, etc.)
 * 5. Compila y verifica que no haya errores
 * 6. Prueba cada función individualmente
 * 
 * ⚠️ IMPORTANTE:
 * - No todos los ejemplos son compatibles entre sí (conflictos de pines)
 * - Algunos requieren hardware adicional
 * - Lee bien los comentarios antes de implementar
 * - Prueba de uno en uno para evitar problemas
 * 
 * 💡 TIPS:
 * - Guarda una copia del código antes de modificar
 * - Usa el Monitor Serie para debug
 * - Implementa cambios graduales
 * - Documenta tus modificaciones
 */
