/*
 * ESP32-CAM Robot Control System
 * 
 * Sistema de control completo para robot con ESP32-CAM AI Thinker
 * Características:
 * - Streaming de video MJPEG
 * - Control de 2 motores DC con driver L298N
 * - Control de LED flash integrado
 * - Servidor web local con interfaz HTML embebida
 * 
 * Hardware:
 * - ESP32-CAM (AI Thinker)
 * - Driver L298N
 * - 2 Motores DC
 * 
 * Pines utilizados:
 * GPIO 12, 13, 14, 15 - Control de motores
 * GPIO 4 - LED Flash
 */

#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// ===========================================
// CONFIGURACIÓN DE RED
// ===========================================
const char* ssid = "Robot-ESP32-CAM"; // Nombre de la red creada por el ESP32
const char* password = "robot-1234";  // Contraseña de la red (mínimo 8 caracteres)

// ===========================================
// CONFIGURACIÓN DE PINES - MOTORES L298N
// ===========================================
#define MOTOR_LEFT_FORWARD   12   // IN1
#define MOTOR_LEFT_BACKWARD  13   // IN2
#define MOTOR_RIGHT_FORWARD  14   // IN3
#define MOTOR_RIGHT_BACKWARD 15   // IN4

// LED Flash
#define LED_FLASH 4

// ===========================================
// CONFIGURACIÓN DE CÁMARA - AI THINKER
// ===========================================
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// ===========================================
// SERVIDOR WEB
// ===========================================
WebServer server(80);

// ===========================================
// VARIABLES GLOBALES
// ===========================================
bool ledState = false;

// ===========================================
// FUNCIONES DE CONTROL DE MOTORES
// ===========================================

void setupMotors() {
  // Configurar todos los pines de motor como salidas
  pinMode(MOTOR_LEFT_FORWARD, OUTPUT);
  pinMode(MOTOR_LEFT_BACKWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_FORWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_BACKWARD, OUTPUT);
  
  // Inicializar todos los pines en LOW
  stopMotors();
  
  Serial.println("Motores inicializados correctamente");
}

void stopMotors() {
  digitalWrite(MOTOR_LEFT_FORWARD, LOW);
  digitalWrite(MOTOR_LEFT_BACKWARD, LOW);
  digitalWrite(MOTOR_RIGHT_FORWARD, LOW);
  digitalWrite(MOTOR_RIGHT_BACKWARD, LOW);
  Serial.println("Motores detenidos");
}

void moveForward() {
  digitalWrite(MOTOR_LEFT_FORWARD, HIGH);
  digitalWrite(MOTOR_LEFT_BACKWARD, LOW);
  digitalWrite(MOTOR_RIGHT_FORWARD, HIGH);
  digitalWrite(MOTOR_RIGHT_BACKWARD, LOW);
  Serial.println("Moviendo adelante");
}

void moveBackward() {
  digitalWrite(MOTOR_LEFT_FORWARD, LOW);
  digitalWrite(MOTOR_LEFT_BACKWARD, HIGH);
  digitalWrite(MOTOR_RIGHT_FORWARD, LOW);
  digitalWrite(MOTOR_RIGHT_BACKWARD, HIGH);
  Serial.println("Moviendo atrás");
}

void turnLeft() {
  digitalWrite(MOTOR_LEFT_FORWARD, LOW);
  digitalWrite(MOTOR_LEFT_BACKWARD, HIGH);
  digitalWrite(MOTOR_RIGHT_FORWARD, HIGH);
  digitalWrite(MOTOR_RIGHT_BACKWARD, LOW);
  Serial.println("Girando izquierda");
}

void turnRight() {
  digitalWrite(MOTOR_LEFT_FORWARD, HIGH);
  digitalWrite(MOTOR_LEFT_BACKWARD, LOW);
  digitalWrite(MOTOR_RIGHT_FORWARD, LOW);
  digitalWrite(MOTOR_RIGHT_BACKWARD, HIGH);
  Serial.println("Girando derecha");
}

// ===========================================
// FUNCIONES DE CONTROL DE LED
// ===========================================

void setupLED() {
  pinMode(LED_FLASH, OUTPUT);
  digitalWrite(LED_FLASH, HIGH);  // Inicia apagado (logica invertida)
  ledState = false;
  Serial.println("LED inicializado (modo digital - logica invertida)");
}

void ledOn() {
  digitalWrite(LED_FLASH, LOW);   // LOW = Encender (logica invertida)
  ledState = true;
  Serial.println(">>> LED ENCENDIDO (GPIO 4 = LOW) <<<");
}

void ledOff() {
  digitalWrite(LED_FLASH, HIGH);  // HIGH = Apagar (logica invertida)
  ledState = false;
  Serial.println(">>> LED APAGADO (GPIO 4 = HIGH) <<<");
}

// ===========================================
// CONFIGURACIÓN DE CÁMARA
// ===========================================

bool setupCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // Configuración de calidad según PSRAM disponible
  if(psramFound()){
    config.frame_size = FRAMESIZE_VGA;    // 640x480
    config.jpeg_quality = 10;             // 0-63, menor = mejor calidad
    config.fb_count = 2;
    Serial.println("PSRAM detectado - Configuración alta calidad");
  } else {
    config.frame_size = FRAMESIZE_SVGA;   // 800x600
    config.jpeg_quality = 12;
    config.fb_count = 1;
    Serial.println("Sin PSRAM - Configuración estándar");
  }

  // Inicializar cámara
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Error al inicializar cámara: 0x%x\n", err);
    return false;
  }

  // Ajustes adicionales del sensor
  sensor_t * s = esp_camera_sensor_get();
  if (s != NULL) {
    s->set_brightness(s, 0);     // -2 a 2
    s->set_contrast(s, 0);       // -2 a 2
    s->set_saturation(s, 0);     // -2 a 2
    s->set_special_effect(s, 0); // 0 a 6 (0 = Sin efecto)
    s->set_whitebal(s, 1);       // 0 = desactivar, 1 = activar
    s->set_awb_gain(s, 1);       // 0 = desactivar, 1 = activar
    s->set_wb_mode(s, 0);        // 0 a 4
    s->set_exposure_ctrl(s, 1);  // 0 = desactivar, 1 = activar
    s->set_aec2(s, 0);           // 0 = desactivar, 1 = activar
    s->set_ae_level(s, 0);       // -2 a 2
    s->set_aec_value(s, 300);    // 0 a 1200
    s->set_gain_ctrl(s, 1);      // 0 = desactivar, 1 = activar
    s->set_agc_gain(s, 0);       // 0 a 30
    s->set_gainceiling(s, (gainceiling_t)0); // 0 a 6
    s->set_bpc(s, 0);            // 0 = desactivar, 1 = activar
    s->set_wpc(s, 1);            // 0 = desactivar, 1 = activar
    s->set_raw_gma(s, 1);        // 0 = desactivar, 1 = activar
    s->set_lenc(s, 1);           // 0 = desactivar, 1 = activar
    s->set_hmirror(s, 0);        // 0 = desactivar, 1 = activar
    s->set_vflip(s, 0);          // 0 = desactivar, 1 = activar
    s->set_dcw(s, 1);            // 0 = desactivar, 1 = activar
    s->set_colorbar(s, 0);       // 0 = desactivar, 1 = activar
  }

  Serial.println("Cámara inicializada correctamente");
  
  // Re-configurar GPIO 4 como salida para LED (logica invertida)
  pinMode(LED_FLASH, OUTPUT);
  digitalWrite(LED_FLASH, HIGH);  // HIGH = apagado en logica invertida
  Serial.println("GPIO 4 (LED) re-inicializado tras cámara (logica invertida)");
  
  return true;
}

// ===========================================
// STREAMING DE VIDEO
// ===========================================

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

void handleStream() {
  camera_fb_t * fb = NULL;
  WiFiClient client = server.client();
  
  if (!client.connected()) {
    Serial.println("Cliente no conectado");
    return;
  }
  
  // Enviar encabezado HTTP
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: " + String(_STREAM_CONTENT_TYPE) + "\r\n";
  response += "Access-Control-Allow-Origin: *\r\n";
  response += "\r\n";
  client.print(response);
  
  Serial.println("Stream iniciado");
  
  while (client.connected()) {
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Error capturando frame");
      break;
    }
    
    // Enviar boundary
    client.print(_STREAM_BOUNDARY);
    
    // Enviar encabezado de parte
    char part_buf[64];
    snprintf(part_buf, 64, _STREAM_PART, fb->len);
    client.print(part_buf);
    
    // Enviar imagen
    client.write(fb->buf, fb->len);
    
    // Liberar buffer
    esp_camera_fb_return(fb);
    
    // CRÍTICO: Procesar otros requests HTTP mientras transmite
    server.handleClient();
    
    // Pequeña pausa para estabilidad
    delay(1);
  }
  
  Serial.println("Stream finalizado");
}

// ===========================================
// PÁGINA WEB HTML
// ===========================================

const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Robot ESP32-CAM</title>
    <style>
        :root {
            --bg:        #0f1117;
            --surface:   #1a1e2e;
            --panel:     #222638;
            --border:    #2e3450;
            --blue:      #2563eb;
            --blue-lt:   #3b82f6;
            --blue-dim:  #1d4ed8;
            --grey:      #4b5563;
            --grey-lt:   #6b7280;
            --text:      #e2e8f0;
            --text-dim:  #94a3b8;
            --danger:    #dc2626;
            --danger-lt: #ef4444;
            --led-on:    #eab308;
            --led-off:   #374151;
        }

        * { margin: 0; padding: 0; box-sizing: border-box; }

        body {
            font-family: 'Segoe UI', system-ui, sans-serif;
            background: var(--bg);
            color: var(--text);
            min-height: 100vh;
            display: flex;
            flex-direction: column;
            align-items: center;
        }

        /* ── HEADER ── */
        header {
            width: 100%;
            background: var(--surface);
            border-bottom: 1px solid var(--border);
            padding: 14px 24px;
            display: flex;
            align-items: center;
            gap: 12px;
        }
        .header-icon {
            width: 36px; height: 36px;
            background: var(--blue);
            border-radius: 8px;
            display: flex; align-items: center; justify-content: center;
            font-size: 18px;
        }
        header h1 { font-size: 17px; font-weight: 600; letter-spacing: .3px; }
        header p  { font-size: 12px; color: var(--text-dim); margin-top: 1px; }
        .status-dot {
            margin-left: auto;
            display: flex; align-items: center; gap: 6px;
            font-size: 12px; color: var(--text-dim);
        }
        .dot {
            width: 8px; height: 8px; border-radius: 50%;
            background: #22c55e;
            box-shadow: 0 0 6px #22c55e;
        }

        /* ── LAYOUT ── */
        .main {
            width: 100%; max-width: 900px;
            padding: 20px 16px;
            display: grid;
            grid-template-columns: 1fr 320px;
            gap: 16px;
        }

        /* ── PANEL BASE ── */
        .panel {
            background: var(--surface);
            border: 1px solid var(--border);
            border-radius: 12px;
            overflow: hidden;
        }
        .panel-header {
            padding: 12px 16px;
            border-bottom: 1px solid var(--border);
            font-size: 12px;
            font-weight: 600;
            letter-spacing: .8px;
            text-transform: uppercase;
            color: var(--text-dim);
            display: flex; align-items: center; gap: 8px;
        }
        .panel-header span { color: var(--blue-lt); }

        /* ── VIDEO ── */
        .video-wrap {
            grid-row: 1 / 3;
        }
        .video-body {
            position: relative;
            width: 100%;
            padding-top: 75%;
            background: #000;
        }
        #stream {
            position: absolute; inset: 0;
            width: 100%; height: 100%;
            object-fit: contain;
        }
        .video-overlay {
            position: absolute; bottom: 10px; left: 10px;
            background: rgba(0,0,0,.55);
            border: 1px solid var(--border);
            border-radius: 6px;
            padding: 4px 10px;
            font-size: 11px;
            color: var(--text-dim);
            backdrop-filter: blur(4px);
        }

        /* ── CONTROLES MOVIMIENTO ── */
        .dpad {
            display: grid;
            grid-template-columns: repeat(3, 70px);
            grid-template-rows: repeat(3, 70px);
            gap: 8px;
            justify-content: center;
            padding: 20px 16px;
        }
        .btn {
            border: none; border-radius: 10px;
            font-size: 13px; font-weight: 600;
            cursor: pointer;
            transition: background .15s, transform .1s, box-shadow .15s;
            user-select: none;
            -webkit-user-select: none;
            touch-action: manipulation;
            display: flex; flex-direction: column;
            align-items: center; justify-content: center;
            gap: 4px;
            line-height: 1;
        }
        .btn svg { width: 18px; height: 18px; }
        .btn:active { transform: scale(.93); }

        .btn-move {
            background: var(--panel);
            border: 1px solid var(--border);
            color: var(--text);
        }
        .btn-move:hover  { background: var(--grey); border-color: var(--grey-lt); }
        .btn-move.active { background: var(--blue-dim); border-color: var(--blue); color: #fff; }

        .btn-fwd  { grid-column: 2; grid-row: 1; }
        .btn-left { grid-column: 1; grid-row: 2; }
        .btn-stop {
            grid-column: 2; grid-row: 2;
            background: var(--danger);
            border: 1px solid var(--danger-lt);
            color: #fff;
            font-size: 11px;
        }
        .btn-stop:hover { background: var(--danger-lt); }
        .btn-right { grid-column: 3; grid-row: 2; }
        .btn-bwd   { grid-column: 2; grid-row: 3; }

        /* ── LED ── */
        .led-body {
            padding: 16px;
            display: flex;
            gap: 10px;
        }
        .btn-led {
            flex: 1;
            padding: 14px 10px;
            border-radius: 10px;
            border: 1px solid var(--border);
            font-size: 13px; font-weight: 600;
            cursor: pointer;
            transition: background .15s, border-color .15s, transform .1s;
            touch-action: manipulation;
        }
        .btn-led:active { transform: scale(.95); }
        .btn-led-on {
            background: rgba(234,179,8,.12);
            border-color: rgba(234,179,8,.4);
            color: #fde047;
        }
        .btn-led-on:hover {
            background: rgba(234,179,8,.22);
            border-color: #eab308;
        }
        .btn-led-off {
            background: var(--led-off);
            color: var(--text-dim);
        }
        .btn-led-off:hover { background: var(--grey); color: var(--text); }

        /* ── STATUS BAR ── */
        .statusbar {
            grid-column: 1 / -1;
            background: var(--surface);
            border: 1px solid var(--border);
            border-radius: 10px;
            padding: 10px 16px;
            display: flex; align-items: center; gap: 16px;
            font-size: 12px; color: var(--text-dim);
        }
        .statusbar b { color: var(--blue-lt); font-weight: 600; }
        .sep { width: 1px; height: 14px; background: var(--border); }

        /* ── RESPONSIVE ── */
        @media (max-width: 680px) {
            .main { grid-template-columns: 1fr; }
            .video-wrap { grid-row: auto; }
            .dpad { grid-template-columns: repeat(3, 64px); grid-template-rows: repeat(3, 64px); }
        }
    </style>
</head>
<body>

<header>
    <div class="header-icon">&#x1F916;</div>
    <div>
        <h1>ESP32-CAM Robot</h1>
        <p>Panel de control</p>
    </div>
    <div class="status-dot"><div class="dot"></div> EN LINEA</div>
</header>

<div class="main">

    <!-- VIDEO -->
    <div class="panel video-wrap">
        <div class="panel-header"><span>&#9654;</span> TRANSMISION EN VIVO</div>
        <div class="video-body">
            <img id="stream" src="/stream" alt="Sin señal">
            <div class="video-overlay">MJPEG &bull; LIVE</div>
        </div>
    </div>

    <!-- MOVIMIENTO -->
    <div class="panel">
        <div class="panel-header"><span>&#9654;</span> MOVIMIENTO</div>
        <div class="dpad">
            <button class="btn btn-move btn-fwd"
                    onmousedown="cmd('adelante')" onmouseup="cmd('stop')"
                    ontouchstart="cmd('adelante')" ontouchend="cmd('stop')">
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><path d="M12 19V5M5 12l7-7 7 7"/></svg>
                AVANZAR
            </button>
            <button class="btn btn-move btn-left"
                    onmousedown="cmd('izquierda')" onmouseup="cmd('stop')"
                    ontouchstart="cmd('izquierda')" ontouchend="cmd('stop')">
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><path d="M19 12H5M12 19l-7-7 7-7"/></svg>
                IZQ
            </button>
            <button class="btn btn-stop" onclick="cmd('stop')">
                <svg viewBox="0 0 24 24" fill="currentColor"><rect x="5" y="5" width="14" height="14" rx="2"/></svg>
                STOP
            </button>
            <button class="btn btn-move btn-right"
                    onmousedown="cmd('derecha')" onmouseup="cmd('stop')"
                    ontouchstart="cmd('derecha')" ontouchend="cmd('stop')">
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><path d="M5 12h14M12 5l7 7-7 7"/></svg>
                DER
            </button>
            <button class="btn btn-move btn-bwd"
                    onmousedown="cmd('atras')" onmouseup="cmd('stop')"
                    ontouchstart="cmd('atras')" ontouchend="cmd('stop')">
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><path d="M12 5v14M5 12l7 7 7-7"/></svg>
                RETRO
            </button>
        </div>
    </div>

    <!-- LED -->
    <div class="panel">
        <div class="panel-header"><span>&#9654;</span> ILUMINACION</div>
        <div class="led-body">
            <button class="btn-led btn-led-on" onclick="cmd('led_off')">&#9728; ENCENDER</button>
            <button class="btn-led btn-led-off" onclick="cmd('led_on')">&#9679; APAGAR</button>
        </div>
    </div>

    <!-- STATUS BAR -->
    <div class="statusbar">
        Ultimo comando: <b id="lastCmd">—</b>
        <div class="sep"></div>
        <span id="connStatus">Conectado</span>
    </div>

</div>

<script>
    async function cmd(command) {
        const labels = {
            adelante:'Avanzar', atras:'Retroceder',
            izquierda:'Izquierda', derecha:'Derecha',
            stop:'Stop', led_on:'LED OFF', led_off:'LED ON'
        };
        
        console.log('Enviando comando:', command); // Debug
        
        // Resaltar boton activo
        document.querySelectorAll('.btn-move').forEach(b => b.classList.remove('active'));
        const map = {adelante:'btn-fwd', atras:'btn-bwd', izquierda:'btn-left', derecha:'btn-right'};
        if (map[command]) document.querySelector('.' + map[command])?.classList.add('active');

        try {
            const r = await fetch('/' + command);
            console.log('Respuesta:', r.status, r.ok); // Debug
            document.getElementById('lastCmd').textContent = labels[command] || command;
            document.getElementById('connStatus').textContent = r.ok ? 'Conectado' : 'Error ' + r.status;
        } catch(e) {
            console.error('Error fetch:', e); // Debug
            document.getElementById('connStatus').textContent = 'Sin conexion';
        }
    }

    // Touch: prevenir doble-tap zoom
    let lt = 0;
    document.addEventListener('touchend', e => {
        const now = Date.now();
        if (now - lt < 300) e.preventDefault();
        lt = now;
    }, false);

    // Prevenir scroll en botones de movimiento (con mousedown/up)
    document.querySelectorAll('.btn-move, .btn-stop').forEach(b => {
        b.addEventListener('touchstart', e => e.preventDefault(), { passive: false });
    });
    
    // Botones LED: permitir click normal, pero agregar soporte touch
    document.querySelectorAll('.btn-led').forEach(b => {
        b.addEventListener('touchend', function(e) {
            e.preventDefault();
            this.click(); // Dispara el onclick
        }, { passive: false });
    });

    document.getElementById('stream').onerror = function() {
        this.alt = 'Sin señal de video';
    };
</script>
</body>
</html>
)=====";

// ===========================================
// HANDLERS DEL SERVIDOR WEB
// ===========================================

void handleRoot() {
  server.send(200, "text/html", MAIN_page);
}

void handleForward() {
  Serial.println("\n>>> COMANDO WEB RECIBIDO: ADELANTE <<<");
  moveForward();
  server.send(200, "text/plain", "OK");
}

void handleBackward() {
  Serial.println("\n>>> COMANDO WEB RECIBIDO: ATRAS <<<");
  moveBackward();
  server.send(200, "text/plain", "OK");
}

void handleLeft() {
  Serial.println("\n>>> COMANDO WEB RECIBIDO: IZQUIERDA <<<");
  turnLeft();
  server.send(200, "text/plain", "OK");
}

void handleRight() {
  Serial.println("\n>>> COMANDO WEB RECIBIDO: DERECHA <<<");
  turnRight();
  server.send(200, "text/plain", "OK");
}

void handleStop() {
  Serial.println("\n>>> COMANDO WEB RECIBIDO: STOP <<<");
  stopMotors();
  server.send(200, "text/plain", "OK");
}

void handleLedOn() {
  Serial.println("\n>>> COMANDO WEB RECIBIDO: LED ON (APAGAR) <<<");
  ledOn();
  server.send(200, "text/plain", "OK");
}

void handleLedOff() {
  Serial.println("\n>>> COMANDO WEB RECIBIDO: LED OFF (ENCENDER) <<<");
  ledOff();
  server.send(200, "text/plain", "OK");
}

void handleNotFound() {
  server.send(404, "text/plain", "404: No encontrado");
}

// ===========================================
// CONFIGURACIÓN INICIAL
// ===========================================

void setup() {
  // Deshabilitar el detector de brownout
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  
  // Inicializar Serial
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  Serial.println("=================================");
  Serial.println("ESP32-CAM Robot Control System");
  Serial.println("=================================");

  // Crear una red WiFi propia para controlar el robot
  WiFi.mode(WIFI_AP);
  if (!WiFi.softAP(ssid, password)) {
    Serial.println("ERROR: No se pudo iniciar la red WiFi del robot");
    Serial.println("Reiniciando en 3 segundos...");
    delay(3000);
    ESP.restart();
  }
  Serial.print("Red WiFi del robot: ");
  Serial.println(ssid);
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.softAPIP());
  
  // Inicializar motores
  setupMotors();
  
  // Inicializar cámara
  if (!setupCamera()) {
    Serial.println("ERROR: Fallo al inicializar cámara");
    Serial.println("Reiniciando en 3 segundos...");
    delay(3000);
    ESP.restart();
  }
  
  // Inicializar LED (después de cámara para evitar conflicto en GPIO 4)
  setupLED();
  
  // Configurar rutas del servidor
  server.on("/", handleRoot);
  server.on("/stream", handleStream);
  server.on("/adelante", handleForward);
  server.on("/atras", handleBackward);
  server.on("/izquierda", handleLeft);
  server.on("/derecha", handleRight);
  server.on("/stop", handleStop);
  server.on("/led_on", handleLedOn);
  server.on("/led_off", handleLedOff);
  server.onNotFound(handleNotFound);
  
  // Iniciar servidor
  server.begin();
  Serial.println("Servidor HTTP iniciado en puerto 80");
  Serial.println("=================================");
  Serial.println("Sistema listo!");
  Serial.print("Accede desde tu navegador: http://");
  Serial.println(WiFi.softAPIP());
  Serial.println("=================================");
}

// ===========================================
// LOOP PRINCIPAL
// ===========================================

void loop() {
  // Manejar solicitudes del servidor web
  server.handleClient();
  
  // Pequeño delay para estabilidad
  delay(1);
}
