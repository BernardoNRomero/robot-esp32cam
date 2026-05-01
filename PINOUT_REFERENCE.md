# 📌 REFERENCIA DE PINES ESP32-CAM AI THINKER

Guía completa de pines para el modelo AI Thinker ESP32-CAM.

---

## 🔍 PINOUT COMPLETO

```
                    ESP32-CAM AI THINKER
                    ════════════════════

        ┌──────────────────────────────────┐
        │           [ANTENA WiFi]          │
        │                                  │
    5V  ●                                  │  Lado de
   GND  ●          ┌──────────┐           │  la cámara
  IO12  ●          │          │           │
  IO13  ●          │  CÁMARA  │           │
  IO15  ●          │  OV2640  │           │
  IO14  ●          │          │           │
   IO2  ●          └──────────┘           │
   IO4  ●                                  │
  IO16  ●        [LED FLASH]               │
   IO0  ●                                  │
   3V3  ●                                  │
   GND  ●          [ESP32-CAM-MB]          │
   VCC  ●          [Botón RESET]           │
        │                                  │
        └──────────────────────────────────┘
            U0T          U0R
           (TX)         (RX)
```

---

## 📊 TABLA DE PINES

| Pin | Nombre | Función Principal | Uso en Cámara | Disponible | Notas |
|-----|--------|-------------------|---------------|------------|-------|
| GND | Ground | Tierra | - | ✅ | Común |
| 3V3 | 3.3V | Alimentación 3.3V | - | ✅ | No usar para alimentar módulo con cámara |
| 5V | 5V | Alimentación 5V | - | ✅ | Usar esta para alimentar |
| **GPIO 0** | IO0 | Boot/Flash | - | ⚠️ | **EVITAR** - Usado para boot, conflictos |
| **GPIO 1** | U0T/TX0 | UART TX | - | ⚠️ | **EVITAR** - UART Serial |
| **GPIO 2** | IO2 | General | **HSPI_WP** | ⚠️ | Cuidado - usado por SD, pero disponible |
| **GPIO 3** | U0R/RX0 | UART RX | - | ⚠️ | **EVITAR** - UART Serial |
| **GPIO 4** | IO4 | LED Flash | **FLASH** | ❌ | LED Flash integrado |
| **GPIO 5** | IO5 | General | **VSYNC** | ❌ | Usado por cámara |
| **GPIO 12** | IO12 | General | **HSPI_MISO** | ✅ | **DISPONIBLE** - Usado en proyecto |
| **GPIO 13** | IO13 | General | **HSPI_MOSI** | ✅ | **DISPONIBLE** - Usado en proyecto |
| **GPIO 14** | IO14 | General | **HSPI_CLK** | ✅ | **DISPONIBLE** - Usado en proyecto |
| **GPIO 15** | IO15 | General | **HSPI_CS** | ✅ | **DISPONIBLE** - Usado en proyecto |
| **GPIO 16** | IO16 | General | **UART2_RXD** | ✅ | **DISPONIBLE** - Alternativa |
| **GPIO 18** | IO18 | General | **Y2_GPIO** | ❌ | Usado por cámara |
| **GPIO 19** | IO19 | General | **Y3_GPIO** | ❌ | Usado por cámara |
| **GPIO 21** | IO21 | I2C SDA | **Y4_GPIO** | ❌ | Usado por cámara |
| **GPIO 22** | IO22 | I2C SCL | **PCLK** | ❌ | Usado por cámara |
| **GPIO 23** | IO23 | General | **HREF** | ❌ | Usado por cámara |
| **GPIO 25** | IO25 | DAC1 | **VSYNC** | ❌ | Usado por cámara |
| **GPIO 26** | IO26 | DAC2 | **SIOD** | ❌ | Usado por cámara (I2C) |
| **GPIO 27** | IO27 | General | **SIOC** | ❌ | Usado por cámara (I2C) |
| **GPIO 32** | IO32 | General | **PWDN** | ❌ | Power down cámara |
| **GPIO 33** | IO33 | General | - | ⚠️ | Disponible pero limitado |
| **GPIO 34** | IO34 | ADC | - | ✅ | Solo INPUT (sin pullup) |
| **GPIO 35** | IO35 | ADC | **Y9_GPIO** | ❌ | Usado por cámara |
| **GPIO 36** | IO36 | ADC | **Y6_GPIO** | ❌ | Usado por cámara |
| **GPIO 39** | IO39 | ADC | **Y7_GPIO** | ❌ | Usado por cámara |

**Leyenda:**
- ✅ = Totalmente disponible
- ⚠️ = Disponible con precauciones
- ❌ = NO disponible (usado por cámara/sistema)

---

## ✅ PINES SEGUROS PARA PROYECTOS

### Pines Digitales de Propósito General

```
GPIO 12 ━━━━━━━━━━━━━━━━━━━━━━ ✅ MEJOR OPCIÓN
GPIO 13 ━━━━━━━━━━━━━━━━━━━━━━ ✅ MEJOR OPCIÓN  
GPIO 14 ━━━━━━━━━━━━━━━━━━━━━━ ✅ MEJOR OPCIÓN
GPIO 15 ━━━━━━━━━━━━━━━━━━━━━━ ✅ MEJOR OPCIÓN

GPIO 2  ━━━━━━━━━━━━━━━━━━━━━━ ⚠️ Disponible (cuidado con boot)
GPIO 16 ━━━━━━━━━━━━━━━━━━━━━━ ⚠️ Disponible

GPIO 33 ━━━━━━━━━━━━━━━━━━━━━━ ⚠️ Limitado (algunos clones)
GPIO 34 ━━━━━━━━━━━━━━━━━━━━━━ ⚠️ Solo INPUT (ADC)
```

### Uso Recomendado en Este Proyecto

```
┌─────────────────────────────────────────────────┐
│  FUNCIÓN            │  PIN      │  DESTINO      │
├─────────────────────┼───────────┼───────────────┤
│  Motor L Adelante   │  GPIO 12  │  L298N IN1   │
│  Motor L Atrás      │  GPIO 13  │  L298N IN2   │
│  Motor R Adelante   │  GPIO 14  │  L298N IN3   │
│  Motor R Atrás      │  GPIO 15  │  L298N IN4   │
│  LED Flash          │  GPIO 4   │  Integrado   │
└─────────────────────┴───────────┴───────────────┘
```

---

## 🚫 PINES A EVITAR

### Usados por la Cámara (NO TOCAR)

```
GPIO 5, 18, 19, 21, 22, 23, 25, 26, 27, 32, 35, 36, 39
         │
         └─→ Todos conectados al sensor OV2640
```

### Conflictos de Boot

```
GPIO 0  ━━ Debe estar HIGH durante boot normal
            LOW durante programación
            
GPIO 2  ━━ Debe estar LOW durante boot
            (puede usar después con precaución)
            
GPIO 12 ━━ Afecta voltaje flash durante boot
            (generalmente OK después)
```

---

## 🔌 CONFIGURACIONES DE PINES PARA ACCESORIOS

### Opción A: Solo Motores (Este Proyecto)

```
GPIO 12 → IN1 (Motor L Adelante)
GPIO 13 → IN2 (Motor L Atrás)
GPIO 14 → IN3 (Motor R Adelante)
GPIO 15 → IN4 (Motor R Atrás)
GPIO 4  → LED Flash
```

**Pines libres**: GPIO 2, 16, 33, 34

---

### Opción B: Motores + Sensor Ultrasónico

```
GPIO 12 → IN1 (Motor L)
GPIO 13 → IN2 (Motor L)
GPIO 14 → IN3 (Motor R)
GPIO 15 → IN4 (Motor R)
GPIO 16 → TRIG (HC-SR04)
GPIO 2  → ECHO (HC-SR04)
GPIO 4  → LED Flash
```

**Pines libres**: GPIO 33, 34

---

### Opción C: Motores + Servos (Torreta)

```
GPIO 12 → IN1 (Motor L)
GPIO 13 → IN2 (Motor L)
GPIO 14 → IN3 (Motor R)
GPIO 15 → IN4 (Motor R)
GPIO 2  → Servo PAN
GPIO 16 → Servo TILT
GPIO 4  → LED Flash
```

**Pines libres**: GPIO 33, 34

---

### Opción D: Motores PWM + Sensores

```
GPIO 12 → IN1 (Motor L)
GPIO 13 → IN2 (Motor L)
GPIO 14 → IN3 (Motor R)
GPIO 15 → IN4 (Motor R)
GPIO 2  → ENA (PWM Motor L)
GPIO 16 → ENB (PWM Motor R)
GPIO 33 → TRIG (Ultrasónico)
GPIO 34 → Sensor Analógico (batería, etc.)
GPIO 4  → LED Flash
```

---

## 🔋 PINES DE ALIMENTACIÓN

### Voltajes Disponibles

```
PIN     VOLTAJE     USO
─────────────────────────────────────────
5V      5.0V        Alimentación principal
3V3     3.3V        Lógica de bajo consumo
GND     0V          Tierra común
```

### Consumo de Corriente Típico

```
ESP32 (sin cámara):        ~80-100 mA
ESP32 + WiFi activo:       ~150-200 mA
ESP32 + WiFi + Cámara:     ~300-500 mA
LED Flash:                 ~100-150 mA
Total máximo:              ~600-650 mA
```

**⚠️ IMPORTANTE**: 
- Alimentar **SIEMPRE** con 5V cuando uses la cámara
- 3.3V NO es suficiente para cámara + WiFi
- Usar fuente de **2A mínimo**

---

## 🎛️ CARACTERÍSTICAS ESPECIALES DE PINES

### Pines ADC (Entrada Analógica)

```
GPIO 34, 35, 36, 39 ━━ ADC1 (12 bits, 0-3.3V)
GPIO 33             ━━ ADC1 (también digital)
GPIO 32             ━━ ADC1 (usado por cámara)
```

**Uso típico**: Leer sensores analógicos, voltaje de batería

**Ejemplo**:
```cpp
int valor = analogRead(34);  // 0-4095
float voltaje = (valor / 4095.0) * 3.3;
```

### Pines DAC (Salida Analógica)

```
GPIO 25 ━━ DAC1 (usado por cámara)
GPIO 26 ━━ DAC2 (usado por cámara)
```

**No disponibles** en este módulo por uso de cámara.

### Pines Touch

```
GPIO 2, 12, 13, 14, 15 ━━ Soportan Touch Sensor
```

**Ejemplo**:
```cpp
int touchValue = touchRead(12);
if (touchValue < 40) {
  // Detectado toque
}
```

### Pines PWM

```
TODOS los GPIO pueden ser PWM (16 canales)
```

**Ejemplo**:
```cpp
ledcSetup(0, 1000, 8);      // Canal 0, 1kHz, 8 bits
ledcAttachPin(12, 0);        // GPIO 12 al canal 0
ledcWrite(0, 128);           // Duty cycle 50%
```

---

## 🛠️ CONFIGURACIÓN DE HARDWARE COMÚN

### Driver L298N

```
ESP32-CAM          L298N
──────────────────────────
GPIO 12      →    IN1
GPIO 13      →    IN2
GPIO 14      →    IN3
GPIO 15      →    IN4
[GPIO 2]     →    ENA (opcional PWM)
[GPIO 16]    →    ENB (opcional PWM)
GND          →    GND
5V           →    +5V (si usas regulador L298N)
```

### Sensor Ultrasónico HC-SR04

```
ESP32-CAM          HC-SR04
────────────────────────────
GPIO 16      →    TRIG
GPIO 2       →    ECHO
5V           →    VCC
GND          →    GND
```

### Servomotor

```
ESP32-CAM          Servo
────────────────────────────
GPIO 2       →    Señal (naranja/amarillo)
5V           →    VCC (rojo)
GND          →    GND (marrón/negro)
```

**⚠️ Advertencia**: Servos consumen mucha corriente, considerar fuente externa.

### LED Externo

```
ESP32-CAM          LED
────────────────────────────
GPIO 12      →    Resistor 220Ω → LED (+)
GND          →    LED (-)
```

### Buzzer Activo

```
ESP32-CAM          Buzzer
────────────────────────────
GPIO 2       →    (+)
GND          →    (-)
```

---

## ⚡ LIMITACIONES Y PRECAUCIONES

### Corriente Máxima por Pin

```
Corriente por GPIO:     40 mA máximo
Corriente total pines:  200 mA máximo (todos combinados)
```

**⚠️ NO conectar cargas altas directamente**:
- Motores → Usar driver (L298N, TB6612)
- Relés → Usar transistor/optoacoplador
- LEDs potentes → Usar transistor

### Voltaje de Pines

```
Nivel ALTO (HIGH):  3.3V
Nivel BAJO (LOW):   0V
Tolerancia:         NO es 5V tolerante
```

**⚠️ NO conectar señales de 5V directamente**:
- Usar divisor de voltaje
- O usar level shifter
- O módulos con lógica 3.3V

### Resistencias Pull-up/Pull-down Internas

```
GPIO con pull-up:     0, 2, 4, 12-15, 16, 33
GPIO solo INPUT:      34, 35, 36, 39 (sin pull-up/down)
```

---

## 📱 PINES DE PROGRAMACIÓN

### UART0 (Programación)

```
U0T (GPIO 1)  ━━ TX ━━ Conectar a RX del FTDI
U0R (GPIO 3)  ━━ RX ━━ Conectar a TX del FTDI
```

### Modo Bootloader

```
Para entrar en modo programación:
1. Conectar GPIO 0 a GND
2. Presionar RESET (o aplicar alimentación)
3. GPIO 0 debe estar LOW durante boot

Para modo normal:
1. GPIO 0 debe estar HIGH o flotante
2. Presionar RESET
```

---

## 🗺️ MAPA DE MEMORIA

### Pines y Funciones Multiplexadas

```
PIN    │ FUNC1    │ FUNC2     │ FUNC3      │ FUNC4
───────┼──────────┼───────────┼────────────┼────────────
GPIO0  │ Boot     │ ADC2_1    │ TOUCH1     │ -
GPIO2  │ Boot     │ ADC2_2    │ TOUCH2     │ SD_D0
GPIO4  │ ADC2_0   │ TOUCH0    │ -          │ LED_FLASH
GPIO12 │ ADC2_5   │ TOUCH5    │ HSPI_MISO  │ SD_D2
GPIO13 │ ADC2_4   │ TOUCH4    │ HSPI_MOSI  │ SD_D3
GPIO14 │ ADC2_6   │ TOUCH6    │ HSPI_CLK   │ SD_CLK
GPIO15 │ ADC2_3   │ TOUCH3    │ HSPI_CS    │ SD_CMD
GPIO16 │ -        │ UART2_RXD │ -          │ -
GPIO33 │ ADC1_5   │ TOUCH8    │ -          │ -
GPIO34 │ ADC1_6   │ -         │ INPUT ONLY │ -
```

---

## 📊 DIAGRAMA DE BLOQUES

```
                    ╔════════════════════════════╗
                    ║      ESP32-CAM Module      ║
                    ╚════════════════════════════╝
                              │
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
   ┌────▼────┐         ┌──────▼──────┐       ┌─────▼─────┐
   │  WiFi   │         │   Cámara    │       │   GPIO    │
   │ Antena  │         │   OV2640    │       │  12-15    │
   └─────────┘         └─────────────┘       └─────┬─────┘
                                                    │
                                             ┌──────▼──────┐
                                             │   L298N     │
                                             │   Driver    │
                                             └──────┬──────┘
                                                    │
                                             ┌──────▼──────┐
                                             │   Motores   │
                                             │    DC x2    │
                                             └─────────────┘
```

---

## 🔗 RECURSOS ADICIONALES

### Datasheets

- **ESP32**: https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf
- **OV2640**: https://www.uctronics.com/download/cam_module/OV2640DS.pdf
- **L298N**: https://www.st.com/resource/en/datasheet/l298.pdf

### Pinout Interactivo

- https://randomnerdtutorials.com/esp32-pinout-reference-gpios/

### Foros y Comunidad

- ESP32 Forum: https://www.esp32.com/
- Reddit: https://reddit.com/r/esp32

---

**⚠️ RECORDATORIO FINAL**

Antes de conectar cualquier hardware:
1. ✅ Verificar voltajes (3.3V vs 5V)
2. ✅ Confirmar que el pin está disponible
3. ✅ Revisar consumo de corriente
4. ✅ Usar GND común entre todos los dispositivos
5. ✅ Probar con LED antes de hardware costoso

---

**Última actualización**: Mayo 2026
