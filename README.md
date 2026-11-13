#  Sistema de Control para Robot ESP32-CAM

Sistema completo de control remoto para robot basado en ESP32-CAM con streaming de video en tiempo real y control de motores DC.

> **⚠️ IMPORTANTE - Lee esto primero**:
> - **Configuración Arduino IDE**: Debes usar **"AI Thinker ESP32-CAM"** con **Partition Scheme "Huge APP"** o el código no compilará.
> - **Drivers USB-TTL**: Si usas **CP2102**, necesitas instalar drivers oficiales de Silicon Labs antes de programar.
> - **ESP32 Core**: Usa versiones **2.0.11 a 2.0.14** (estables). Evita 3.x.
> - **Programación sin FTDI**: Si tienes Arduino Uno, NO necesitas comprar programador. Ver [guía](PROGRAMAR_CON_ARDUINO_UNO.md).

##  Tabla de Contenidos

- [Características](#características)
- [Requisitos de Hardware](#requisitos-de-hardware)
- [Diagrama de Conexiones](#diagrama-de-conexiones)
- [Instalación del Software](#instalación-del-software)
- [Configuración](#configuración)
- [Cómo Usar](#cómo-usar)
- [Solución de Problemas](#solución-de-problemas)
- [Optimizaciones Avanzadas](#optimizaciones-avanzadas)

---

##  Características

- ✅ **Streaming de video MJPEG** en tiempo real desde la ESP32-CAM
- ✅ **Control bidireccional** de 2 motores DC mediante driver L298N
- ✅ **Control del LED flash** integrado de la ESP32-CAM
- ✅ **Interfaz web responsive** optimizada para móviles y PC
- ✅ **Control continuo** mientras se mantiene presionado el botón
- ✅ **Sin backend externo** - todo corre en la ESP32
- ✅ **Código profesional** con manejo de errores y comentarios

---

##  Requisitos de Hardware

### Componentes Necesarios

| Componente | Cantidad | Notas |
|------------|----------|-------|
| ESP32-CAM (AI Thinker) | 1 | Con módulo OV2640 |
| Driver L298N | 1 | Controlador de motores |
| Motores DC | 2 | 3-12V recomendado |
| Fuente de alimentación | 1 | 5V/2A mínimo (para ESP32 y lógica) |
| Fuente de alimentación motores | 1 | 6-12V según motores (para L298N) |
| Cables Dupont | Varios | Macho-hembra y macho-macho |
| Programador FTDI/USB-TTL | 1 | Para cargar código (o usa Arduino Uno - ver nota) |

> **💡 Nota Importante**: Si tienes un Arduino Uno/Nano/Mega, **NO necesitas comprar un programador FTDI**. Puedes usar tu Arduino como programador. Ver guía completa en [PROGRAMAR_CON_ARDUINO_UNO.md](PROGRAMAR_CON_ARDUINO_UNO.md)

### Especificaciones Técnicas

- **ESP32-CAM**: Módulo con WiFi integrado y cámara OV2640
- **L298N**: Soporta hasta 2A por canal, voltaje de motor 5-35V
- **Alimentación ESP32-CAM**: 5V (no usar 3.3V con cámara activa)

---

##  Diagrama de Conexiones

### Conexiones ESP32-CAM ↔ L298N

```
ESP32-CAM          →    L298N Driver
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
GPIO 12           →    IN1 (Motor Izquierdo Adelante)
GPIO 13           →    IN2 (Motor Izquierdo Atrás)
GPIO 14           →    IN3 (Motor Derecho Adelante)
GPIO 15           →    IN4 (Motor Derecho Atrás)
GND               →    GND (Común)
```

### Conexiones L298N ↔ Motores

```
L298N             →    Motores
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
OUT1              →    Motor Izquierdo (+)
OUT2              →    Motor Izquierdo (-)
OUT3              →    Motor Derecho (+)
OUT4              →    Motor Derecho (-)
```

### Alimentación

```
IMPORTANTE: Sistema de doble alimentación
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

1. ALIMENTACIÓN LÓGICA (ESP32-CAM):
   Fuente 5V/2A  →  5V y GND de ESP32-CAM
   
2. ALIMENTACIÓN MOTORES (L298N):
   Batería 6-12V  →  +12V y GND de L298N
   
   NOTA: Puedes usar el +5V del L298N para alimentar
   la ESP32-CAM si el regulador del L298N soporta
   suficiente corriente (verificar datasheet)

3. TIERRA COMÚN:
   ⚠️ MUY IMPORTANTE: Conectar los GND de ambas 
   fuentes juntos (común) para que las señales
   funcionen correctamente.
```

### Diagrama Visual

```
                    ┌─────────────────┐
                    │   ESP32-CAM     │
                    │   (AI Thinker)  │
                    │                 │
                    │  GPIO12 ────────┼─────→ IN1
                    │  GPIO13 ────────┼─────→ IN2    ┌──────────┐
 Fuente 5V ────────→│  5V             │              │  L298N   │
                    │  GND   ─────────┼──────┬──────→│  Driver  │
                    │  GPIO14 ────────┼──────┘  ┌───→│  GND     │
                    │  GPIO15 ────────┼─────────┼───→│  IN3     │
                    │                 │         │    │  IN4     │
                    │  GPIO4 = LED    │         │    │          │
                    └─────────────────┘         │    │  OUT1────┼─→ M1+
                                                │    │  OUT2────┼─→ M1-
 Batería Motores (6-12V)                        │    │  OUT3────┼─→ M2+
    (+) ───────────────────────────────────────→│    │  OUT4────┼─→ M2-
    (-) ───────────────────────────────────────→│    │  +12V    │
                                          GND común  │  GND     │
                                                     └──────────┘
```

### ⚠️ ADVERTENCIAS IMPORTANTES

1. **NO conectar GPIO 0, 1, 2, 3** - Se usan para la cámara
2. **GPIO 4** ya está en uso por el LED flash integrado
3. **NO alimentar ESP32-CAM con 3.3V** cuando la cámara está activa
4. **Siempre conectar GND común** entre ESP32 y L298N
5. **Verificar polaridad** antes de alimentar

---

##  Instalación del Software

### Paso 1: Instalar Arduino IDE

1. Descargar Arduino IDE desde: https://www.arduino.cc/en/software
2. Instalar versión 2.x o superior (recomendado)

### Paso 2: Instalar Drivers USB-TTL (CP2102)

⚠️ **CRÍTICO**: Si usas un programador **CP2102 USB to UART Bridge Controller** (muy común), necesitas instalar los drivers oficiales.

#### Instalación de Drivers CP2102:

1. **Descargar driver oficial de Silicon Labs**:
   - 🔗 Link: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
   - Buscar: **CP210x Universal Windows Driver**
   - Descargar el instalador (.exe para Windows)

2. **Instalar el driver**:
   - Ejecutar el instalador descargado
   - Seguir el asistente (Next → Next → Install)
   - Reiniciar la PC si se solicita

3. **Verificar instalación**:
   - Conectar el programador USB-TTL al PC
   - Ir a **Administrador de Dispositivos** (Windows + X → Administrador de dispositivos)
   - Buscar en **Puertos (COM y LPT)**
   - Debe aparecer: **Silicon Labs CP210x USB to UART Bridge (COMx)**
   - Anotar el número de puerto (ejemplo: COM3, COM4, etc.)

> 💡 **Síntoma de drivers faltantes**: Si al conectar el USB-TTL aparece "Dispositivo desconocido" o "Error de comunicación serial", necesitas instalar/actualizar los drivers.

### Paso 3: Configurar ESP32 en Arduino IDE

1. Abrir Arduino IDE
2. Ir a `Archivo` → `Preferencias`
3. En "Gestor de URLs Adicionales de Tarjetas", agregar:
   ```
   https://dl.espressif.com/dl/package_esp32_index.json
   ```
   > **Nota**: Si ya tienes otras URLs, sepáralas con coma
   
4. Ir a `Herramientas` → `Placa` → `Gestor de Tarjetas`
5. Buscar "**esp32**"
6. Instalar "**esp32 by Espressif Systems**"
   - ✅ **Versión recomendada**: 2.0.11 a 2.0.14 (estables y probadas)
   - ⚠️ Evitar versiones 3.x (pueden tener incompatibilidades)
7. Esperar a que termine la instalación (puede tardar varios minutos)
8. **Reiniciar Arduino IDE** para que los cambios tengan efecto

### Paso 4:  Configuración OBLIGATORIA de la Placa

Esta configuración es **crítica** para que el código compile y funcione correctamente.

1. **Seleccionar la placa**:
   - `Herramientas` → `Placa` → `ESP32 Arduino` → **"AI Thinker ESP32-CAM"**
   - 💡 **Tip**: Está en una lista larga. En IDE 2.x puedes buscar "AI Thinker" en el buscador

2. **⚠️ CONFIGURACIÓN OBLIGATORIA (copiar exactamente)**:

   ```
   ┌─────────────────────────────────────────────────────────┐
   │ Herramientas → Configuración                            │
   ├─────────────────────────────────────────────────────────┤
   │ Board:              "AI Thinker ESP32-CAM"              │
   │ Upload Speed:       "115200"                            │
   │ Flash Frequency:    "80MHz"                             │
   │ Flash Mode:         "QIO"                               │
   │ Partition Scheme:   "Huge APP (3MB No OTA/1MB SPIFFS)"  │
   │ Core Debug Level:   "Ninguno" (o "None")                │
   │ Puerto:             COMx (el que detectó Windows)       │
   └─────────────────────────────────────────────────────────┘
   ```

3. **Verificar configuración antes de compilar**:
   - Si usas configuración incorrecta, el código puede compilar pero fallar en ejecución
   - **Partition Scheme "Huge APP"** es necesario para que el HTML embebido quepa en memoria

> ⚠️ **Error común**: Si aparece error "Sketch too big" o "not enough space", verifica que Partition Scheme sea **"Huge APP (3MB No OTA/1MB SPIFFS)"**

### Paso 5: Preparar Hardware para Programación

> **💡 ¿No tienes programador USB-TTL?** Si tienes un Arduino Uno/Nano/Mega, puedes usarlo como programador. Ver guía completa en [PROGRAMAR_CON_ARDUINO_UNO.md](PROGRAMAR_CON_ARDUINO_UNO.md)

#### Opción A: Con USB-TTL (CP2102, FTDI, CH340)

1. **Conectar el programador a la ESP32-CAM**:
   ```
   USB-TTL (CP2102)    ESP32-CAM
   ────────────────────────────────
   5V            →     5V
   GND           →     GND
   TX            →     U0R (RX)
   RX            →     U0T (TX)
   ```

2. **Entrar en modo programación**:
   - **Conectar GPIO0 a GND** (con un jumper o cable)
   - **Presionar RESET** en la ESP32-CAM (si tiene botón)
   - O conectar y desconectar la alimentación
   - La ESP32 ahora está en modo bootloader

3. **Verificar puerto COM**:
   - En Arduino IDE: `Herramientas` → `Puerto`
   - Debe aparecer algo como: `COM3 (Silicon Labs CP210x USB to UART Bridge)`
   - Si no aparece, revisar instalación de drivers (Paso 2)

#### Opción B: Con Arduino Uno (sin programador dedicado)

Ver guía detallada: [PROGRAMAR_CON_ARDUINO_UNO.md](PROGRAMAR_CON_ARDUINO_UNO.md)

### Paso 6: Cargar el Código

1. Abrir el archivo `ESP32_CAM_Robot_Control.ino`
2. **Configurar la red WiFi propia del robot (opcional)**:
   El ESP32-CAM crea su propia red al arrancar; no necesita conectarse a un router.
   Si quieres cambiar el nombre o la contraseña, edita estos valores:
   ```cpp
   const char* ssid = "Robot-ESP32-CAM"; // Nombre de la red
   const char* password = "robot-1234";  // Mínimo 8 caracteres
   ```
3. **Verificar el código**: Clic en ✓ (Verificar)
   - Debe compilar sin errores
   - Si aparece "Sketch too big", revisar Partition Scheme en Paso 4
4. **Cargar el código**: Clic en → (Subir)
   - Asegurarse que GPIO0 está conectado a GND
   - Esperar mensaje "Done uploading" o "Subida exitosa"
5. Si falla la subida, ver [Solución de Problemas](#solución-de-problemas)

### Paso 7: Arrancar el Sistema

1. **Desconectar GPIO0 de GND**
2. Presionar el botón **RESET** de la ESP32-CAM
3. Abrir el **Monitor Serie** (115200 baudios)
4. Deberías ver algo como:
   ```
   =================================
   ESP32-CAM Robot Control System
   =================================
   Red WiFi del robot: Robot-ESP32-CAM
   Dirección IP: 192.168.4.1
   Motores inicializados correctamente
   PSRAM detectado - Configuración alta calidad
   LED inicializado
   Servidor HTTP iniciado en puerto 80
   =================================
   Sistema listo!
   Accede desde tu navegador: http://192.168.4.1
   =================================
   ```

---

##  Cómo Usar

### Acceder a la Interfaz Web

1. **Conectar tu PC/móvil a la red `Robot-ESP32-CAM`** creada por el ESP32-CAM.
   - Contraseña predeterminada: `robot-1234`
   - Esta red es independiente de tu router y no proporciona acceso a internet.
2. **Abrir un navegador** (Chrome, Firefox, Safari, etc.)
3. Ir a `http://192.168.4.1` (también se muestra en el Monitor Serie).
4. **La página se cargará** mostrando:
   - Video en vivo de la cámara
   - Controles de movimiento
   - Controles del LED

### Controles de Movimiento

| Botón | Acción | Uso |
|-------|--------|-----|
| **▲ Adelante** | Ambos motores hacia adelante | Mantener presionado para avanzar |
| **▼ Atrás** | Ambos motores hacia atrás | Mantener presionado para retroceder |
| **◄ Izquierda** | Motor izq. atrás + Motor der. adelante | Mantener para girar |
| **► Derecha** | Motor izq. adelante + Motor der. atrás | Mantener para girar |
| **■ STOP** | Detiene todos los motores | Clic para detención inmediata |

### Controles del LED

- ** Encender LED**: Activa el flash de la cámara
- ** Apagar LED**: Desactiva el flash

### Funcionamiento

- **Control continuo**: Los botones direccionales funcionan mientras los mantienes presionados
- **Auto-stop**: Al soltar el botón, el robot se detiene automáticamente
- **Streaming + Control simultáneo**: Los comandos responden inmediatamente incluso durante la transmisión de video
- **Streaming**: El video se actualiza automáticamente sin refrescar la página
- **Mobile-friendly**: Funciona perfectamente en smartphones y tablets
- **LED con lógica invertida**: El código ya está configurado para clones ESP32-CAM comunes

> 💡 **Tip**: Abre la consola del navegador (F12) para ver mensajes de debug y confirmar que los comandos se envían correctamente.

---

##  Solución de Problemas

### Problema: No se puede cargar el código

**Síntomas**: Error "Failed to connect to ESP32", "No serial data received", "Could not open port"

**Soluciones**:
1. ✅ Verificar que GPIO0 esté conectado a GND antes de subir
2. ✅ Presionar RESET mientras GPIO0 está a GND
3. ✅ Verificar conexiones USB-TTL (TX↔RX cruzados)
4. ✅ Probar con velocidad de subida más baja (115200 → 9600)
5. ✅ Verificar que el puerto COM sea el correcto
6. ✅ Cerrar Monitor Serial antes de subir código
7. ✅ **Instalar drivers CP2102/CH340** (ver siguiente sección)

### Problema: Puerto COM no aparece o "Acceso denegado"

**Síntomas**: No aparece puerto COM, error "PermissionError", "Access Denied"

**Soluciones CP2102 (Silicon Labs)**:
1. ✅ **Descargar driver oficial**: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
2. ✅ **Desinstalar driver antiguo** (si lo tenías):
   - Panel de Control → Programas → Desinstalar
   - Buscar "Silicon Labs" o "CP210x"
3. ✅ **Instalar driver nuevo**:
   - Ejecutar instalador descargado
   - Reiniciar PC
4. ✅ **Verificar en Administrador de Dispositivos**:
   - Windows + X → Administrador de dispositivos
   - Puertos (COM y LPT)
   - Debe aparecer: "Silicon Labs CP210x USB to UART Bridge (COMx)"
   - Si aparece con ⚠️ amarillo: clic derecho → Actualizar controlador

**Soluciones CH340 (alternativo)**:
1. ✅ Descargar driver CH340: http://www.wch-ic.com/downloads/CH341SER_ZIP.html
2. ✅ Instalar y reiniciar
3. ✅ Verificar que aparezca "USB-SERIAL CH340 (COMx)"

**Puerto ocupado/bloqueado**:
1. ✅ Cerrar **Monitor Serial** de Arduino IDE
2. ✅ Cerrar **PuTTY, Tera Term, Hercules** u otros programas seriales
3. ✅ Cerrar otras instancias de Arduino IDE
4. ✅ Desconectar y reconectar el USB-TTL
5. ✅ Reiniciar Arduino IDE

### Problema: "Sketch too big" al compilar

**Síntomas**: Error "Sketch uses XXX bytes, exceeds maximum"

**Soluciones**:
1. ✅ **CRÍTICO**: Verificar en `Herramientas` → `Partition Scheme`:
   - Debe ser: **"Huge APP (3MB No OTA/1MB SPIFFS)"**
   - Si dice "Default" o "Minimal SPIFFS", cambiar a Huge APP
2. ✅ Verificar que la placa sea **"AI Thinker ESP32-CAM"**
3. ✅ Si persiste, reducir tamaño del HTML en el código

### Problema: ESP32 se reinicia constantemente

**Síntomas**: Bootloop, reinicios constantes

**Soluciones**:
1. ✅ **Alimentación insuficiente** - Usar fuente de al menos 5V/2A
2. ✅ **Cable USB malo** - Probar con otro cable de calidad
3. ✅ **Consumo de cámara** - La cámara consume mucho, verificar alimentación
4. ✅ Conectar capacitor 100-470µF entre 5V y GND (cerca de la ESP32-CAM)

### Problema: No aparece la red WiFi del robot o no puedo conectarme

**Síntomas**: No aparece `Robot-ESP32-CAM` en la lista de redes o falla la conexión.

**Soluciones**:
1. ✅ Verificar que la ESP32-CAM esté alimentada correctamente y haya terminado de arrancar.
2. ✅ Buscar el nombre configurado en `ssid` (distingue mayúsculas y minúsculas).
3. ✅ Usar la contraseña configurada en `password` (mínimo 8 caracteres).
4. ✅ Acercar el dispositivo a la ESP32-CAM; el alcance depende del entorno y la antena.
5. ✅ Reiniciar la ESP32-CAM si la red no aparece.

### Problema: Cámara no inicializa

**Síntomas**: "Error al inicializar cámara: 0x..."

**Soluciones**:
1. ✅ **Verificar voltaje** - Debe ser 5V (no 3.3V)
2. ✅ **Revisar flat cable** de la cámara (conexión firme)
3. ✅ **No usar GPIO 0,1,2,3** para otros componentes
4. ✅ Probar con otra configuración de resolución en el código:
   ```cpp
   config.frame_size = FRAMESIZE_QVGA;  // 320x240 (menos exigente)
   ```

### Problema: Página web se carga pero no hay video

**Síntomas**: Controles funcionan pero imagen en negro o no carga

**Soluciones**:
1. ✅ Probar accediendo directamente a: `http://[IP]/stream`
2. ✅ Verificar en Monitor Serie si hay errores de streaming
3. ✅ Reducir calidad/resolución de video:
   ```cpp
   config.frame_size = FRAMESIZE_VGA;
   config.jpeg_quality = 15;  // Mayor número = menor calidad pero más rápido
   ```
4. ✅ Verificar que el navegador soporte MJPEG (todos modernos lo soportan)
5. ✅ Limpiar caché del navegador

### Problema: Motores no responden

**Síntomas**: Los controles web funcionan pero motores no se mueven

**Soluciones**:
1. ✅ **Verificar alimentación del L298N** (6-12V en el terminal de alimentación de motores)
2. ✅ **Verificar conexiones** GPIO 12,13,14,15 a IN1,IN2,IN3,IN4
3. ✅ **GND común** entre ESP32-CAM y L298N
4. ✅ Verificar en Monitor Serie que aparezcan mensajes de "Moviendo adelante", etc.
5. ✅ Probar motores directamente conectándolos a la batería
6. ✅ Verificar que los **jumpers de ENABLE** del L298N estén puestos (ENA, ENB)

### Problema: Robot va en dirección incorrecta

**Síntomas**: Al presionar "Adelante" va hacia atrás, o gira al revés

**Soluciones**:
1. ✅ **Invertir polaridad** de los motores físicamente
2. ✅ O modificar el código, intercambiar HIGH/LOW en las funciones:
   ```cpp
   void moveForward() {
     digitalWrite(MOTOR_LEFT_FORWARD, LOW);   // Invertir
     digitalWrite(MOTOR_LEFT_BACKWARD, HIGH); // estos
     // ... resto igual
   }
   ```

### Problema: LED flash no funciona

**Síntomas**: Botones LED no encienden el flash

**Soluciones**:
1. ✅ **Lógica invertida** (común en clones): El código actual ya usa lógica invertida (LOW=ON, HIGH=OFF)
2. ✅ Verificar en Monitor Serial que aparezca: "LED ENCENDIDO (GPIO 4 = LOW)"
3. ✅ GPIO4 es correcto para AI Thinker (algunos clones usan GPIO33)
4. ✅ Si aún no funciona, probar cambiar el pin en el código:
   ```cpp
   #define LED_FLASH 33  // Probar con 33 si 4 no funciona
   ```
5. ✅ El LED puede estar dañado o no instalado en clones económicos

### Problema: Botones web no responden durante streaming

**Síntomas**: Los controles funcionan antes del video, pero no durante transmisión

**Soluciones**:
1. ✅ **Ya corregido en versión actual**: El código incluye `server.handleClient()` dentro del loop de streaming
2. ✅ Si usas código antiguo, agregar en `handleStream()` dentro del `while`:
   ```cpp
   server.handleClient();  // Procesar comandos HTTP mientras transmite
   ```
3. ✅ Verificar en Monitor Serial que los comandos lleguen: ">>> COMANDO WEB RECIBIDO: ..."
4. ✅ Abrir consola del navegador (F12) y verificar errores JavaScript

### Problema: Conexión web lenta o entrecortada

**Síntomas**: Video se congela, comandos tardan en responder

**Soluciones**:
1. ✅ Acercar el dispositivo de control a la ESP32-CAM
2. ✅ Reducir calidad/resolución de video
3. ✅ No usar múltiples dispositivos simultáneos para ver el stream
4. ✅ Verificar que la señal de la red `Robot-ESP32-CAM` sea estable
5. ✅ Reducir frame rate (aumentar delay en función handleStream):
   ```cpp
   delay(50);  // Cambiar de 10 a 50 ms
   ```

---

## ⚡ Optimizaciones Avanzadas

### Control PWM de Velocidad

Para control de velocidad variable de los motores:

```cpp
// Agregar al inicio del código
#define PWM_FREQ 1000      // Frecuencia PWM en Hz
#define PWM_RESOLUTION 8   // 8 bits = 0-255
#define PWM_CHANNEL_LEFT 0
#define PWM_CHANNEL_RIGHT 1

// Pines de ENABLE del L298N
#define MOTOR_LEFT_ENABLE 2
#define MOTOR_RIGHT_ENABLE 16

// En setupMotors():
ledcSetup(PWM_CHANNEL_LEFT, PWM_FREQ, PWM_RESOLUTION);
ledcSetup(PWM_CHANNEL_RIGHT, PWM_FREQ, PWM_RESOLUTION);
ledcAttachPin(MOTOR_LEFT_ENABLE, PWM_CHANNEL_LEFT);
ledcAttachPin(MOTOR_RIGHT_ENABLE, PWM_CHANNEL_RIGHT);

// Función para controlar velocidad
void setMotorSpeed(int speed) { // 0-255
  ledcWrite(PWM_CHANNEL_LEFT, speed);
  ledcWrite(PWM_CHANNEL_RIGHT, speed);
}
```

### Mejorar Calidad de Video

```cpp
// En setupCamera(), ajustar:
config.frame_size = FRAMESIZE_SVGA;  // 800x600
config.jpeg_quality = 8;              // Menor = mejor (rango 0-63)
config.fb_count = 2;                  // Doble buffer
```

### Agregar Control de Velocidad en la Web

Añadir slider en HTML:

```html
<input type="range" min="0" max="255" value="200" 
       id="speedSlider" onchange="setSpeed(this.value)">

<script>
function setSpeed(value) {
  fetch(`/speed?value=${value}`);
}
</script>
```

Y handler en Arduino:

```cpp
void handleSpeed() {
  if (server.hasArg("value")) {
    int speed = server.arg("value").toInt();
    setMotorSpeed(speed);
    server.send(200, "text/plain", "OK");
  }
}

// En setup():
server.on("/speed", handleSpeed);
```

### Cambiar la IP del punto de acceso (opcional)

Por defecto, la red creada por el ESP32-CAM usa `192.168.4.1`. Para asignarle otra IP,
configúrala antes de llamar a `WiFi.softAP()` en `setup()`:

```cpp
IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
  Serial.println("Error configurando la IP del punto de acceso");
}
WiFi.softAP(ssid, password);
```

### Agregar Autenticación Básica

```cpp
// Al inicio del código
const char* www_username = "admin";
const char* www_password = "robot123";

// Modificar handlers:
void handleRoot() {
  if (!server.authenticate(www_username, www_password)) {
    return server.requestAuthentication();
  }
  server.send(200, "text/html", MAIN_page);
}
```

---

##  Crear Acceso Directo en Móvil

### Android

1. Abrir Chrome y acceder a `http://[IP]`
2. Menú (⋮) → "Añadir a pantalla de inicio"
3. Nombrar "Robot ESP32"
4. Icono aparecerá como app

### iOS

1. Abrir Safari y acceder a `http://[IP]`
2. Botón compartir (□↑)
3. "Añadir a pantalla de inicio"
4. Nombrar "Robot ESP32"

---

##  Especificaciones del Sistema

| Característica | Valor |
|----------------|-------|
| **Resolución video** | 640x480 (VGA) |
| **Frame rate** | ~15-25 FPS (depende de WiFi) |
| **Latencia control** | 50-150ms |
| **Alcance WiFi** | 10-30m aproximadamente (depende del entorno y la antena) |
| **Consumo ESP32-CAM** | ~300mA (streaming activo) |
| **Consumo motores** | Depende de motores (típico 200-500mA cada uno) |

---

##  Conceptos Técnicos

### ¿Cómo funciona el streaming MJPEG?

MJPEG (Motion JPEG) envía una secuencia continua de imágenes JPEG:
```
HTTP Headers → JPEG1 → Boundary → JPEG2 → Boundary → ...
```
- No es un video comprimido (como H.264)
- Cada frame es una imagen JPEG completa
- Simple pero consume más ancho de banda que H.264

### ¿Por qué usar L298N?

- El ESP32 solo proporciona 3.3V y ~40mA por GPIO
- Los motores DC necesitan más voltaje (6-12V) y corriente (>500mA)
- El L298N es un puente H doble que permite:
  - Controlar dirección (adelante/atrás)
  - Manejar alta corriente (hasta 2A por canal)
  - Separar alimentación lógica de alimentación de potencia

### Pinout ESP32-CAM AI Thinker

```
                    ┌─────────┐
          5V ──────→│●       │
         GND ──────→│●       │
        IO12 ──────→│●       │
        IO13 ──────→│●   E   │
        IO15 ──────→│●   S   │
        IO14 ──────→│●   P   │
         IO2 ──────→│●   3   │──→ IO4 (LED)
         IO4 ──────→│●   2   │──→ UOR
        IO16 ──────→│●   -   │──→ UOT
         IO0 ──────→│●   C   │──→ IO1
         3V3 ──────→│●   A   │
         GND ──────→│●   M   │──→ VCC
                    └─────────┘
```

---

##  Advertencias de Seguridad

⚠️ **IMPORTANTE - LEER ANTES DE USAR**

1. **Polaridad**: Invertir polaridad puede dañar componentes
2. **Voltaje**: No alimentar ESP32-CAM con más de 5.5V
3. **Corriente**: Verificar que tu fuente soporte la carga total
4. **Cortocircuito**: Evitar cables sueltos que puedan tocarse
5. **Calor**: El L298N puede calentarse - ventilación adecuada
6. **WiFi del robot**: La red del punto de acceso usa contraseña; cámbiala si es necesario y comparte el acceso solo con usuarios de confianza. La red no tiene acceso a internet.
7. **Control**: Mantener línea de vista - el robot puede moverse de forma impredecible

---

##  Soporte y Recursos

### Recursos Adicionales

- [Documentación ESP32-CAM](https://github.com/espressif/esp32-camera)
- [Datasheet L298N](https://www.st.com/resource/en/datasheet/l298.pdf)
- [Arduino ESP32 Reference](https://docs.espressif.com/projects/arduino-esp32/en/latest/)

### Comunidades

- [ESP32 Forum](https://www.esp32.com/)
- [Arduino Forum](https://forum.arduino.cc/)
- [Reddit r/esp32](https://www.reddit.com/r/esp32/)

---

## � Archivos del Proyecto

Este proyecto incluye múltiples guías y recursos:

| Archivo | Descripción |
|---------|-------------|
| **ESP32_CAM_Robot_Control.ino** | Código principal del robot - ¡Comienza aquí! |
| **README.md** | Documentación completa (este archivo) |
| **QUICK_START.md** | Guía de inicio rápido - Listo en 15 minutos |
| **PROGRAMAR_CON_ARDUINO_UNO.md** | 📟 Cómo programar usando Arduino Uno (sin FTDI) |
| **FAQ.md** | Preguntas frecuentes con respuestas detalladas |
| **ADVANCED_CONFIG.md** | Configuraciones avanzadas y expansiones |
| **CODE_EXAMPLES.ino** | 20+ ejemplos de código adicionales |
| **PINOUT_REFERENCE.md** | Referencia completa de pines ESP32-CAM |

**¿Nuevo en el proyecto?** Sigue este orden:
1. **QUICK_START.md** - Para poner en marcha rápido
2. **PROGRAMAR_CON_ARDUINO_UNO.md** - Si no tienes FTDI
3. **FAQ.md** - Si tienes problemas
4. **CODE_EXAMPLES.ino** - Para agregar funciones

---

## � Licencia

Este proyecto es de código abierto y puede ser utilizado libremente para proyectos educativos y personales.

---

## ✅ Checklist Final

Antes de considerar el proyecto completado, verifica:

- [ ] Código cargado exitosamente en ESP32-CAM
- [ ] ESP32-CAM crea la red `Robot-ESP32-CAM` y muestra su IP
- [ ] PC/móvil conectado a esa red abre `http://192.168.4.1`
- [ ] Página web se carga correctamente en el navegador
- [ ] Video streaming funciona sin cortes
- [ ] Botón "Adelante" mueve el robot hacia adelante
- [ ] Botón "Atrás" mueve el robot hacia atrás
- [ ] Botón "Izquierda" hace girar el robot a la izquierda
- [ ] Botón "Derecha" hace girar el robot a la derecha
- [ ] Botón "STOP" detiene inmediatamente los motores
- [ ] LED flash se enciende y apaga correctamente
- [ ] Control funciona desde smartphone
- [ ] Todas las conexiones están aisladas y seguras

---

Si tienes algún problema, revisa la sección de [Solución de Problemas](#solución-de-problemas) o consulta las comunidades mencionadas.

---

**Última actualización**: Mayo 2026
**Versión**: 1.0.0
