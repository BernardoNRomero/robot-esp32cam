# ⚡ GUÍA DE INICIO RÁPIDO - 15 MINUTOS

Esta guía te llevará paso a paso desde el desempaquetado hasta tener tu robot funcionando.

---

## ✅ CHECKLIST ANTES DE EMPEZAR

- [ ] ESP32-CAM (AI Thinker)
- [ ] Driver L298N
- [ ] 2 Motores DC
- [ ] Programador FTDI/USB-TTL (o Arduino Uno - ver nota abajo)
- [ ] Cables Dupont (al menos 8)
- [ ] Fuente 5V/2A
- [ ] Batería/fuente 6-12V para motores
- [ ] Cable USB para el FTDI (o Arduino Uno)
- [ ] PC con Arduino IDE instalado

> **💡 ¿Tienes un Arduino Uno?** No necesitas comprar programador FTDI. Ver [PROGRAMAR_CON_ARDUINO_UNO.md](PROGRAMAR_CON_ARDUINO_UNO.md) para usar tu Arduino como programador.

---

## 📍 PASO 1: CONEXIONES (5 minutos)

### 1.1 ESP32-CAM a L298N

```
ESP32-CAM   →   L298N
─────────────────────
GPIO 12     →   IN1
GPIO 13     →   IN2
GPIO 14     →   IN3
GPIO 15     →   IN4
GND         →   GND
```

### 1.2 L298N a Motores

```
L298N       →   Motores
───────────────────────
OUT1        →   Motor Izquierdo (+)
OUT2        →   Motor Izquierdo (-)
OUT3        →   Motor Derecho (+)
OUT4        →   Motor Derecho (-)
```

### 1.3 Alimentación

```
OPCIÓN A (Más simple):
─────────────────────
Fuente 5V (+)  →  ESP32-CAM (5V)
Fuente 5V (-)  →  ESP32-CAM (GND) y L298N (GND)
Batería (+)    →  L298N (+12V)
Batería (-)    →  L298N (GND)

OPCIÓN B (Un solo cable):
──────────────────────────
Batería 9-12V (+)  →  L298N (+12V)
Batería (-)        →  L298N (GND)
L298N (+5V salida) →  ESP32-CAM (5V)
L298N (GND)        →  ESP32-CAM (GND)
```

**⚠️ CRÍTICO**: Verificar que los GND estén conectados entre todos los componentes.

### 1.4 Verificación Visual

```
      ┌──────────────┐
      │  ESP32-CAM   │
      └──┬───────┬───┘
         │       │
      GPIO12-15  5V/GND
         │       │
      ┌──┴───────┴─────────┐
      │      L298N         │
      │  IN1 IN2 IN3 IN4   │
      └──┬───────┬─────┬───┘
         │       │     │
      OUT1-4  +12V   GND
         │       │     │
      Motores Batería Común
```

---

## 💻 PASO 2: CONFIGURAR ARDUINO IDE (3 minutos)

### 2.1 Abrir Arduino IDE

1. Ejecutar Arduino IDE
2. Ir a `Archivo` → `Preferencias`

### 2.2 Agregar ESP32

En "URLs Adicionales de Gestor de Tarjetas", pegar:
```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

Click `OK`

### 2.3 Instalar Plataforma ESP32

1. `Herramientas` → `Placa` → `Gestor de Tarjetas`
2. Buscar: "esp32"
3. Instalar: "esp32 by Espressif Systems"
4. Esperar (puede tardar 5+ minutos)

### 2.4 Seleccionar Placa

```
Herramientas → Placa → ESP32 Arduino → "AI Thinker ESP32-CAM"
```

---

## 🔧 PASO 3: PREPARAR CÓDIGO (2 minutos)

### 3.1 Abrir el Archivo

1. Abrir `ESP32_CAM_Robot_Control.ino`

### 3.2 Modificar WiFi

**BUSCAR ESTAS LÍNEAS (aprox. línea 30):**

```cpp
const char* ssid = "TU_SSID";           
const char* password = "TU_PASSWORD";
```

**CAMBIAR POR TUS DATOS:**

```cpp
const char* ssid = "MiRedWiFi";           // Tu red WiFi
const char* password = "MiContraseña123";  // Tu contraseña
```

**⚠️ IMPORTANTE**: 
- Usar comillas dobles `""`
- WiFi debe ser 2.4GHz (no 5GHz)
- Respetar mayúsculas/minúsculas

### 3.3 Verificar Código

Click en ✓ (Verificar/Compilar)

**Esperar mensaje**: "Compilación terminada"

---

## 📤 PASO 4: CARGAR CÓDIGO (3 minutos)

### 4.1 Conectar FTDI a ESP32-CAM

```
FTDI      ESP32-CAM
─────────────────
5V    →   5V
GND   →   GND
TX    →   U0R (RX)
RX    →   U0T (TX)
```

### 4.2 Modo Programación

1. **Conectar GPIO0 a GND** con un jumper/cable
2. **Conectar FTDI al PC** vía USB
3. **Presionar RESET** en ESP32-CAM (si tiene botón)

### 4.3 Configurar Puerto

```
Herramientas → Puerto → [Seleccionar puerto COM/ttyUSB]
```

**Windows**: COM3, COM4, etc.
**Linux/Mac**: /dev/ttyUSB0, etc.

### 4.4 Cargar

1. Click en → (Subir)
2. **ESPERAR** (1-2 minutos)
3. Ver progreso en barra inferior

**Debe decir**: "Done uploading" o "Carga completada"

### 4.5 Modo Normal

1. **DESCONECTAR GPIO0 de GND**
2. **Presionar RESET**
3. Ahora está en modo ejecución

---

## 📡 PASO 5: VERIFICAR FUNCIONAMIENTO (2 minutos)

### 5.1 Abrir Monitor Serie

1. `Herramientas` → `Monitor Serie`
2. Seleccionar: **115200 baudios** (esquina inferior derecha)

### 5.2 Leer IP

Deberías ver algo como:

```
=================================
ESP32-CAM Robot Control System
=================================
LED inicializado
Motores inicializados correctamente
PSRAM detectado - Configuración alta calidad
Cámara inicializada correctamente
Conectando a WiFi.....
WiFi conectado exitosamente
Dirección IP: 192.168.1.100
Señal WiFi: -45 dBm
Servidor HTTP iniciado en puerto 80
=================================
Sistema listo!
Accede desde tu navegador: http://192.168.1.100
=================================
```

**⚠️ SI NO VES ESTO**: Ver sección de Problemas abajo

### 5.3 Anotar IP

**Apuntar la IP mostrada**, ejemplo:
```
192.168.1.100
```

---

## 🌐 PASO 6: PROBAR EN NAVEGADOR (2 minutos)

### 6.1 Conectar a la Misma Red

- Tu PC/móvil debe estar en la **misma red WiFi**
- Verificar que estás en 2.4GHz si tienes red dual

### 6.2 Abrir Navegador

1. Abrir Chrome, Firefox, Safari, etc.
2. Escribir en la barra de direcciones:
   ```
   http://192.168.1.100
   ```
   (usar TU IP del paso 5.2)

### 6.3 Verificar Página

Deberías ver:
- ✅ Título "Robot ESP32-CAM"
- ✅ Video en vivo (puede tardar 2-3 segundos en aparecer)
- ✅ Botones de control
- ✅ Controles de LED

### 6.4 Probar Controles

**Sin motores conectados todavía**:
1. Click en botón "▲ Adelante"
2. Ver en Monitor Serie: "Moviendo adelante"
3. Click en "■ STOP"
4. Ver en Monitor Serie: "Motores detenidos"
5. Probar LED ON/OFF

---

## 🤖 PASO 7: PROBAR MOTORES

### 7.1 Verificación Final de Conexiones

Antes de alimentar:
- [ ] ESP32-CAM GPIO 12-15 conectados a L298N IN1-4
- [ ] Motores conectados a L298N OUT1-4
- [ ] GND común entre todos los componentes
- [ ] Batería de motores conectada a L298N +12V
- [ ] Jumpers ENA y ENB del L298N puestos (si no usas PWM)

### 7.2 Alimentar Motores

1. **Conectar batería** al L298N
2. Verificar que **LEDs del L298N** se enciendan

### 7.3 Prueba de Movimiento

En la página web:

1. **Presionar y mantener** "▲ Adelante"
   - Los motores deben girar
   - Si giran pero en dirección incorrecta, ver nota abajo
   
2. **Soltar botón**
   - Motores deben detenerse

3. **Probar otros botones**:
   - ▼ Atrás
   - ◄ Izquierda
   - ► Derecha

### 7.4 Ajustar Dirección (si es necesario)

Si los motores giran al revés:

**Opción A (Hardware)**: Invertir cables del motor en el L298N
- OUT1 ↔ OUT2 para motor izquierdo
- OUT3 ↔ OUT4 para motor derecho

**Opción B (Software)**: Modificar código (ver ADVANCED_CONFIG.md)

---

## 🎉 ¡LISTO! TU ROBOT FUNCIONA

Tu robot ESP32-CAM está completamente operativo.

### Próximos pasos:

1. **Guardar tu IP favorita**: Crear acceso directo en el móvil
2. **Montar en chasis**: Poner todo en una estructura física
3. **Experimentar**: Probar diferentes configuraciones
4. **Personalizar**: Ver CODE_EXAMPLES.ino para más funciones
5. **Agregar sensores**: HC-SR04, servos, etc.

---

## ❌ SOLUCIÓN RÁPIDA DE PROBLEMAS

### No se carga el código

```
1. Verificar GPIO0 conectado a GND
2. Presionar RESET mientras está conectado
3. Probar con velocidad 9600 en lugar de 115200
4. Verificar cables FTDI (TX-RX cruzados)
5. Instalar drivers CH340/CP2102
```

### No aparece nada en Monitor Serie

```
1. Verificar baudios = 115200
2. Presionar RESET de ESP32-CAM
3. Desconectar GPIO0 de GND
4. Verificar alimentación 5V
```

### No se conecta a WiFi

```
1. Verificar SSID y password correctos
2. Asegurarse que es red 2.4GHz (no 5GHz)
3. Acercar ESP32 al router
4. Probar con hotspot del móvil
5. Verificar que red no tenga filtrado MAC
```

### Página web no carga

```
1. Verificar que estás en la misma red WiFi
2. Usar la IP exacta mostrada en Monitor Serie
3. Probar desde otro dispositivo
4. Presionar RESET y verificar IP de nuevo
5. Limpiar caché del navegador
```

### Video no aparece

```
1. Esperar 3-5 segundos (tarda en cargar)
2. Verificar cable flat de cámara bien conectado
3. Verificar alimentación 5V estable
4. Reducir resolución en código
5. Probar directamente: http://[IP]/stream
```

### Motores no responden

```
1. Verificar alimentación de L298N (6-12V)
2. Verificar LEDs del L298N encendidos
3. Verificar jumpers ENA/ENB puestos
4. Verificar GND común
5. Probar conectar motor directo a batería
6. Ver en Monitor Serie si llegan comandos
```

### ESP32 se reinicia solo

```
1. CAUSA #1: Alimentación insuficiente
   → Usar fuente de 2A mínimo
   → Cable USB corto y grueso
   
2. Agregar capacitor 470µF entre 5V y GND
3. No usar motores y cámara simultáneamente al principio
4. Verificar voltaje con multímetro
```

---

## 📋 TABLA DE VERIFICACIÓN FINAL

Marca cada ítem cuando lo hayas completado:

**Hardware:**
- [ ] ESP32-CAM alimentada con 5V
- [ ] GPIO 12-15 conectados a L298N
- [ ] Motores conectados a L298N
- [ ] GND común entre todos los componentes
- [ ] Batería de motores conectada

**Software:**
- [ ] Arduino IDE configurado
- [ ] Placa ESP32 instalada
- [ ] Código modificado con tu WiFi
- [ ] Código cargado exitosamente
- [ ] GPIO0 desconectado después de cargar

**Funcionamiento:**
- [ ] ESP32 se conecta a WiFi
- [ ] IP mostrada en Monitor Serie
- [ ] Página web se carga en navegador
- [ ] Video en vivo funciona
- [ ] Botones responden
- [ ] LED flash funciona
- [ ] Motores giran

---

## 🎯 COMANDOS ÚTILES

### URLs de Acceso Rápido

```
Página principal:    http://[TU_IP]/
Stream de video:     http://[TU_IP]/stream
Diagnósticos:        http://[TU_IP]/diagnostics (si implementaste)
Logs:               http://[TU_IP]/logs (si implementaste)
```

### Monitor Serie - Mensajes Normales

```
✅ "WiFi conectado exitosamente"
✅ "Cámara inicializada correctamente"
✅ "Motores inicializados correctamente"
✅ "Servidor HTTP iniciado"
✅ "Moviendo adelante" / "Girando izquierda" etc.
```

### Monitor Serie - Mensajes de Error

```
❌ "Error al inicializar cámara" → Problema hardware cámara
❌ "Failed to connect" → No puede conectar WiFi
❌ "Brownout detector" → Alimentación insuficiente
```

---

## 📞 SOPORTE

Si después de seguir esta guía tienes problemas:

1. **Revisar FAQ.md** - Respuestas a preguntas comunes
2. **Ver README.md** - Documentación completa
3. **Consultar ADVANCED_CONFIG.md** - Configuraciones avanzadas
4. **Buscar en Google** - "[Tu error] ESP32-CAM"
5. **Foros**: esp32.com, reddit.com/r/esp32

---

## 📚 ARCHIVOS DEL PROYECTO

```
📁 Robot/
│
├── 📄 ESP32_CAM_Robot_Control.ino       ← ARCHIVO PRINCIPAL
├── 📄 README.md                          ← Documentación completa
├── 📄 QUICK_START.md                    ← Esta guía (estás aquí)
├── 📄 PROGRAMAR_CON_ARDUINO_UNO.md      ← 📟 Usa Arduino Uno como programador
├── 📄 FAQ.md                             ← Preguntas frecuentes
├── 📄 ADVANCED_CONFIG.md                 ← Configuración avanzada
├── 📄 CODE_EXAMPLES.ino                  ← Ejemplos de código adicionales
└── 📄 PINOUT_REFERENCE.md                ← Referencia de pines ESP32-CAM
```

---

## 🎓 PRÓXIMOS PASOS

Una vez que tengas todo funcionando:

1. **Personalizar interfaz**: Cambiar colores, agregar botones
2. **Agregar sensores**: HC-SR04, seguidor de línea, etc.
3. **Control PWM**: Velocidad variable de motores
4. **Modo autónomo**: Evitación de obstáculos
5. **Torreta móvil**: Servos para mover la cámara
6. **Mejorar alcance**: Antena externa WiFi

Ver **CODE_EXAMPLES.ino** para ideas y código listo para usar.

---

## ⏱️ TIEMPO TOTAL: ~15 minutos

- Conexiones: 5 min
- Configurar Arduino: 3 min
- Código: 2 min
- Cargar: 3 min
- Verificar: 2 min

**¡Disfruta tu robot ESP32-CAM!** 🤖🎉

---

**Última actualización**: Mayo 2026
