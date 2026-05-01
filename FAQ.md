# ❓ Preguntas Frecuentes (FAQ)

Respuestas a las preguntas más comunes sobre el proyecto Robot ESP32-CAM.

---

## 🔌 Hardware

### ¿Puedo usar otros drivers de motor en lugar del L298N?

**Sí, puedes usar:**

- **TB6612FNG**: Más eficiente, menos calor, menor tamaño
- **DRV8833**: Bueno para motores pequeños
- **L293D**: Menos potente pero más barato
- **BTS7960**: Para motores de alta corriente (hasta 43A)

**Consideraciones**: Solo necesitas cambiar las conexiones de pines y asegurarte de que la lógica sea 3.3V o 5V compatible.

### ¿Cuántos amperios necesita mi fuente de alimentación?

**Cálculo básico**:
- ESP32-CAM: ~300-500mA (con streaming)
- Motores DC: 200-1000mA cada uno (depende del motor)
- Total mínimo: **1.5-2.5A**

**Recomendación**: Usa una fuente de al menos **3A** para tener margen de seguridad.

### ¿Funciona con ESP32-CAM genérico (no AI Thinker)?

**Sí, pero**: Algunos clones tienen diferencias:
- LED flash en GPIO 33 en lugar de GPIO 4
- Sin PSRAM (menos memoria)
- Calidad de cámara inferior

**Solución**: Verifica el pinout de tu modelo específico y ajusta las definiciones.

### ¿Puedo usar motores de 12V?

**Sí**: El L298N soporta motores de 5-35V. Solo asegúrate de:
- Alimentar el L298N con 12V en la entrada correspondiente
- ESP32-CAM sigue con 5V
- GND común entre ambos

### ¿Necesito un programador FTDI?

**No necesariamente**:
- **Arduino Uno/Nano/Mega**: Puedes usarlo como programador (recomendado) - Ver [PROGRAMAR_CON_ARDUINO_UNO.md](PROGRAMAR_CON_ARDUINO_UNO.md)
- **Programador FTDI**: Opción dedicada ($3-6)
- **Adaptador USB incluido**: Algunos módulos ESP32-CAM vienen con adaptador
- **Cables USB-TTL genéricos**: CP2102, CH340, etc.

**Recomendación**: Si ya tienes un Arduino Uno, úsalo como programador siguiendo la guía específica.

---

## 📡 Conectividad

### ¿A qué distancia funciona el WiFi?

**Respuesta típica**: 10-30 metros en interior, hasta 100m en exterior sin obstáculos.

**Factores que afectan**:
- Potencia del router
- Interferencias (microondas, Bluetooth)
- Paredes y obstáculos
- Antena de la ESP32-CAM

**Mejoras**:
- Usar router con mejor señal
- Agregar antena externa (si tu módulo lo permite)
- Usar modo AP para control directo

### ¿Puedo controlar el robot desde internet?

**No directamente por seguridad**. Necesitarías:
1. **VPN**: Configurar VPN en tu router (más seguro)
2. **Port Forwarding**: Abrir puerto 80 en el router (no recomendado, inseguro)
3. **Cloudflare Tunnel**: Servicio gratuito pero requiere configuración adicional
4. **ngrok**: Túnel temporal (solución rápida para pruebas)

**Recomendación**: Si necesitas control remoto, usa una VPN tipo Tailscale o WireGuard.

### ¿Por qué el ESP32 no se conecta a mi WiFi?

**Causas comunes**:
1. **WiFi de 5GHz**: ESP32 solo soporta 2.4GHz
2. **Contraseña incorrecta**: Verifica mayúsculas/minúsculas
3. **Canal WiFi**: Algunos canales (12, 13) no son compatibles
4. **SSID oculto**: ESP32 no puede ver redes ocultas por defecto
5. **Seguridad WPA3**: Cambiar router a WPA2

**Solución**: Configura tu router en 2.4GHz, WPA2, canales 1-11.

### ¿Puedo conectar múltiples dispositivos a la vez?

**Sí**, la ESP32 puede manejar múltiples conexiones:
- 4-5 clientes simultáneos para controles
- Solo 1 cliente para streaming (limitación del protocolo)

**Nota**: Cada conexión adicional aumenta la carga y puede causar lag.

---

## 📹 Cámara y Video

### ¿Por qué el video va lento o se congela?

**Causas y soluciones**:

1. **WiFi débil**: Acércate al router o usa mejor antena
2. **Resolución muy alta**: Reduce a VGA o CIF
3. **Calidad muy alta**: Aumenta `jpeg_quality` a 15-20
4. **Sin PSRAM**: Limita resolución a QVGA
5. **Múltiples viewers**: Solo permite 1 stream a la vez

### ¿Puedo grabar el video?

**Sí, desde el navegador**:
- Usar herramientas de grabación de pantalla
- Extensiones de Chrome como "Video DownloadHelper"
- OBS Studio para grabación profesional

**Directamente en ESP32**: Muy limitado por memoria, no recomendado.

### ¿Cómo mejoro la calidad de imagen?

```cpp
// En setupCamera():
s->set_brightness(s, 1);      // Aumentar si está oscuro
s->set_contrast(s, 1);        // Mejorar contraste
s->set_saturation(s, 1);      // Colores más vivos
s->set_hmirror(s, 1);         // Espejo horizontal si está invertido
s->set_vflip(s, 1);           // Voltear verticalmente
```

### ¿Por qué la imagen está invertida?

```cpp
// En setupCamera(), después de obtener el sensor:
sensor_t * s = esp_camera_sensor_get();
s->set_hmirror(s, 1);  // Espejo horizontal
s->set_vflip(s, 1);    // Voltear vertical
```

### ¿Puedo tomar fotos?

**Sí, agregar este endpoint**:

```cpp
void handleCapture() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    server.send(500, "text/plain", "Error capturando");
    return;
  }
  
  server.send_P(200, "image/jpeg", (const char *)fb->buf, fb->len);
  esp_camera_fb_return(fb);
}

// En setup()
server.on("/capture", handleCapture);
```

En HTML:
```html
<button onclick="capturePhoto()">📷 Tomar Foto</button>

<script>
function capturePhoto() {
  window.open('/capture', '_blank');
}
</script>
```

---

## ⚡ Rendimiento

### ¿Por qué la ESP32-CAM se reinicia sola?

**Causas principales**:

1. **Alimentación insuficiente**: Causa #1 de reinicios
   - Solución: Fuente de 5V/2A mínimo, cable corto y grueso
   
2. **Pico de corriente de cámara**: Al inicializar consume mucho
   - Solución: Capacitor de 470µF entre 5V y GND
   
3. **Brownout detector**: Protección contra bajo voltaje
   - Solución: Ya está deshabilitado en el código
   
4. **Sobrecalentamiento**: Uso intensivo sin ventilación
   - Solución: Agregar disipador o ventilador pequeño
   
5. **Conflicto de pines**: Usar GPIO0 o GPIO1 durante boot
   - Solución: Evitar estos pines para accesorios

### ¿Cómo reduzco el lag en los controles?

**Optimizaciones**:

1. Reducir calidad/resolución de video (consume ancho de banda)
2. Usar WiFi menos congestionado (cambiar canal del router)
3. Acercarse físicamente al router
4. Eliminar delay en loop() si no causa problemas
5. Usar cable Ethernet en tu PC/móvil (si es posible)

### ¿Puedo aumentar el FPS del video?

**Sí, varias formas**:

1. **Reducir resolución**: QVGA da más FPS que VGA
2. **Aumentar jpeg_quality** (paradójicamente): Mayor valor = menos compresión = menos CPU
3. **Reducir delay** en handleStream():
   ```cpp
   delay(5);  // o incluso eliminar
   ```
4. **Overclock** (no recomendado): 
   ```cpp
   setCpuFrequencyMhz(240);  // Máximo
   ```

---

## 🎮 Control y Uso

### ¿Puedo usar un gamepad real (Bluetooth)?

**No directamente en ESP32**, pero sí desde el navegador:

**Usando Gamepad API del navegador**:
```html
<script>
window.addEventListener("gamepadconnected", (e) => {
  console.log("Gamepad conectado:", e.gamepad);
  gameLoop();
});

function gameLoop() {
  const gamepads = navigator.getGamepads();
  const gp = gamepads[0];
  
  if (!gp) {
    requestAnimationFrame(gameLoop);
    return;
  }
  
  // Analog stick izquierdo
  let x = gp.axes[0];  // -1 a 1
  let y = gp.axes[1];
  
  if (Math.abs(y) > 0.3) {
    if (y < 0) sendCommand('adelante');
    else sendCommand('atras');
  } else if (Math.abs(x) > 0.3) {
    if (x < 0) sendCommand('izquierda');
    else sendCommand('derecha');
  } else {
    sendCommand('stop');
  }
  
  requestAnimationFrame(gameLoop);
}
</script>
```

### ¿Funciona en iOS/iPhone?

**Sí, pero con limitaciones**:
- Safari funciona perfectamente
- Necesitas usar eventos `touchstart`/`touchend` (ya implementados)
- No puedes instalar como PWA real sin certificado HTTPS
- Puedes agregar a pantalla inicio como bookmark

### ¿Por qué los motores siguen girando después de soltar el botón?

**Posibles causas**:

1. **JavaScript no detecta touchend**: Verificar implementación en móvil
2. **Conexión lenta**: El comando 'stop' tarda en llegar
3. **L298N con jumpers ENA/ENB**: Si usas PWM, debes quitarlos

**Solución temporal**:
```javascript
// Agregar timeout de seguridad
let stopTimeout;
function sendCommand(cmd) {
  clearTimeout(stopTimeout);
  fetch(`/${cmd}`);
  
  if (cmd !== 'stop') {
    stopTimeout = setTimeout(() => sendCommand('stop'), 500);
  }
}
```

### ¿Puedo controlar varios robots a la vez?

**Sí, dos opciones**:

**Opción 1: Diferentes IPs**
- Cada robot tiene su propia IP
- Abrir múltiples pestañas del navegador

**Opción 2: Sistema multi-robot (avanzado)**
- Un ESP32 como coordinador
- Comunicación ESP-NOW entre módulos
- Requiere código adicional

---

## 🔧 Problemas Específicos

### El LED flash no funciona

**Diagnóstico**:

1. **Verificar pin**: GPIO 4 en AI Thinker, GPIO 33 en algunos clones
2. **Probar directamente**:
   ```cpp
   void setup() {
     pinMode(4, OUTPUT);
     digitalWrite(4, HIGH);
     delay(1000);
     digitalWrite(4, LOW);
   }
   ```
3. **LED quemado**: Puede estar dañado (raro pero posible)

### Los motores giran muy lento

**Causas**:

1. **Voltaje insuficiente**: Motores de 12V con batería de 6V
2. **Jumpers ENA/ENB removidos**: Necesitan PWM o estar puestos
3. **Cable muy largo**: Resistencia del cable reduce voltaje
4. **Batería descargada**: Medir voltaje con multímetro

**Soluciones**:
- Usar voltaje adecuado para tus motores
- Si usas control simple (sin PWM), poner jumpers ENA/ENB
- Usar cables más gruesos y cortos

### Un motor gira y el otro no

**Diagnóstico paso a paso**:

1. **Verificar motor**: Conectar directamente a batería
2. **Verificar L298N**: LEDs indicadores deben encender
3. **Verificar conexiones**: GPIO correctos a IN1-4
4. **Verificar código**: Ver mensajes en Serial Monitor
5. **Probar intercambiar**: Conectar motor "bueno" en salida "mala"

### Error "Brownout detector was triggered"

**Ya está deshabilitado en el código**, pero si persiste:

```cpp
// Al inicio de setup()
WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  // Ya incluido

// Además, reducir consumo:
// 1. No usar LED y motores al mismo tiempo
// 2. Reducir resolución de cámara
// 3. Usar mejor fuente de alimentación
```

---

## 📚 Expansión y Mejoras

### ¿Puedo agregar más sensores?

**Sí, pines disponibles**:
- GPIO 2, 16 (más seguros)
- GPIO 33 (si no usas para LED)

**Sensores compatibles**:
- HC-SR04 (ultrasónico): 2 pines
- DHT11/22 (temperatura): 1 pin
- PIR (movimiento): 1 pin
- IR receiver: 1 pin
- Servomotores: 1 pin cada uno

### ¿Puedo controlar servomotores para una torreta?

**Sí, ejemplo**:

```cpp
#include <ESP32Servo.h>

Servo panServo;
Servo tiltServo;

void setup() {
  panServo.attach(2);   // GPIO 2
  tiltServo.attach(16); // GPIO 16
}

void handleServo() {
  if (server.hasArg("pan")) {
    int angle = server.arg("pan").toInt();
    panServo.write(angle);
  }
  if (server.hasArg("tilt")) {
    int angle = server.arg("tilt").toInt();
    tiltServo.write(angle);
  }
  server.send(200, "text/plain", "OK");
}
```

### ¿Puedo hacer que siga una línea?

**Sí, necesitas**:
- Sensores TCRT5000 (2-3 unidades)
- Código de lógica PID o simple
- Superficie con línea negra en fondo blanco

**Código básico**:
```cpp
#define SENSOR_LEFT 2
#define SENSOR_RIGHT 16

void followLine() {
  bool left = digitalRead(SENSOR_LEFT);
  bool right = digitalRead(SENSOR_RIGHT);
  
  if (!left && !right) {
    moveForward();
  } else if (left && !right) {
    turnLeft();
  } else if (!left && right) {
    turnRight();
  } else {
    stopMotors();
  }
}
```

### ¿Puedo guardar configuraciones en memoria?

**Sí, usando EEPROM o Preferences**:

```cpp
#include <Preferences.h>

Preferences preferences;

void saveConfig() {
  preferences.begin("robot", false);
  preferences.putInt("speed", currentSpeed);
  preferences.putString("ssid", ssid);
  preferences.end();
}

void loadConfig() {
  preferences.begin("robot", true);
  currentSpeed = preferences.getInt("speed", 200);  // 200 por defecto
  preferences.end();
}
```

---

## 🔒 Seguridad

### ¿Es seguro exponer esto a internet?

**NO directamente**. El sistema no tiene:
- Autenticación robusta
- Encriptación HTTPS
- Protección contra ataques
- Rate limiting

**Para internet, necesitas**:
1. VPN privada
2. Certificados SSL/TLS
3. Autenticación OAuth2
4. Firewall y rate limiting

**Para uso local**: Perfectamente seguro en tu red privada.

### ¿Cómo agrego una contraseña fuerte?

```cpp
const char* www_username = "admin";
const char* www_password = "TuContraseñaSuperSegura123!";

bool checkAuth() {
  if (!server.authenticate(www_username, www_password)) {
    server.requestAuthentication(DIGEST_AUTH);  // Más seguro que BASIC
    return false;
  }
  return true;
}
```

**Nota**: Incluso con autenticación, la contraseña viaja en texto plano sin HTTPS.

---

## 💰 Costos

### ¿Cuánto cuesta armar este proyecto?

**Estimación (precios aproximados)**:

| Componente | Precio USD |
|------------|-----------|
| ESP32-CAM | $8-12 |
| L298N | $3-5 |
| Motores DC (x2) | $4-8 |
| Programador FTDI | $3-6 |
| Fuente 5V/2A | $5-8 |
| Cables y varios | $5-10 |
| **Total** | **$28-49** |

**Más barato**: ~$25 comprando en AliExpress/Banggood
**Más caro**: ~$60 comprando en tienda local con envío rápido

### ¿Vale la pena comprar un kit completo?

**Ventajas de kits**:
- Todo incluido
- Componentes compatibles garantizados
- A veces incluye chasis

**Desventajas**:
- Más caro (20-30% más)
- Menos flexibilidad
- Calidad variable

**Recomendación**: Si es tu primer proyecto, compra un kit. Si tienes experiencia, compra componentes sueltos.

---

## 🎓 Aprendizaje

### ¿Necesito saber programar para usar esto?

**Para usar tal cual**: No, solo:
1. Cambiar SSID y contraseña WiFi
2. Cargar el código
3. Abrir en navegador

**Para personalizar**: Sí, necesitas:
- Conocimientos básicos de C/C++
- HTML/CSS/JavaScript para la interfaz
- Electrónica básica

### ¿Dónde aprendo más?

**Recursos recomendados**:
- [Random Nerd Tutorials](https://randomnerdtutorials.com/projects-esp32-cam/) - Tutoriales ESP32-CAM
- [ESP32 Forum](https://www.esp32.com/) - Comunidad oficial
- [Arduino Reference](https://www.arduino.cc/reference/en/) - Documentación Arduino
- YouTube: "ESP32-CAM tutorial"

### ¿Qué otros proyectos puedo hacer con ESP32-CAM?

**Ideas**:
1. Cámara de seguridad con detección de movimiento
2. Time-lapse automático
3. Reconocimiento facial (con TensorFlow Lite)
4. Sistema de visión para drones
5. Baby monitor con notificaciones
6. Cámara para mascotas con dispensador de comida
7. Sistema de aparcamiento con guía visual
8. Scanner de documentos portátil

---

## 🆘 Soporte

### ¿Dónde consigo ayuda si tengo problemas?

1. **Revisar este FAQ** y el README.md
2. **Monitor Serie**: Ver mensajes de error
3. **Google**: Buscar el error específico
4. **Foros**:
   - ESP32 Forum
   - Arduino Forum
   - Reddit r/esp32
5. **GitHub Issues**: Reportar bugs específicos del código

### ¿Cómo reporto un bug?

**Incluir**:
1. Modelo exacto de ESP32-CAM
2. Versión de Arduino IDE y librería ESP32
3. Código modificado (si aplica)
4. Mensajes de error completos del Monitor Serie
5. Descripción clara del problema
6. Qué ya intentaste hacer

---

**¿Tienes otra pregunta?** Revisa la documentación completa o consulta en las comunidades mencionadas.

---

**Última actualización**: Mayo 2026
